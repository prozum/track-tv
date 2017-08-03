#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <typeinfo>

#include <unistd.h>

extern "C" {
#include <firmata.h>
}

using namespace cv;
using namespace std;

#define DIPIN 2 // Direction pin
#define ACPIN 3 // Activate pin
#define PNPIN 4 // Pne pin

#define KEY_ESC     27
#define KEY_P       116

#define WIDTH       800
#define HEIGHT      600
#define STOPAREA    40
#define CHECKROUNDS 3
#define TIME        200000

int main(int argc, char *argv[])
{
    int rounds_ac, rounds_di;
    int total_x, total_y;
    int average_x, average_y;

    int rounds = 0;
    int key = -1;
    bool pnstate = false;

    vector<Rect> faces;
    vector<Rect>::const_iterator i;

    CascadeClassifier cas;
    Mat src_img, gray_img, eq_img;

    t_firmata *firmata;

    // Check arguments
    if (argc < 2) {
        cout << "Please specify a cascade file." << endl;
        return 1;
    }

    // Load cascade file
    if  (!cas.load(string(argv[1]))) {
        cout << "Cannot read cascade file: " << argv[1] << endl;
        return 1;
    }

    // Init Firmata
    firmata = firmata_new((char*)"/dev/ttyACM99");

    // Wait until device is up
    while(!firmata->isReady) {
        firmata_pull(firmata);
    }

    // Direction pin
    firmata_pinMode(firmata, DIPIN, MODE_OUTPUT);

    // Activate pin
    firmata_pinMode(firmata, ACPIN, MODE_OUTPUT);

    // Pneumatic cylinder pin
    firmata_pinMode(firmata, PNPIN, MODE_OUTPUT);
  
    // Setup window
    namedWindow("track-tv", WINDOW_NORMAL);

    // Use 4 theads in OpenCV
    setNumThreads(4);
    // Use optimizations in OpenCV
    cvUseOptimized(1);

    // Open the default camera
    VideoCapture cam(0);
    if(!cam.isOpened()) {
        cout << "Cannot open cam!" << endl;
        return -1;
    }

    do
    {
        // Get key input
        key = waitKey(1);

        // Get a new frame from camera
        cam >> src_img;

        // Simplify image for cas analysis
        cvtColor(src_img, gray_img, CV_RGB2GRAY);
        equalizeHist(gray_img, eq_img);

        // Analyse image
        cas.detectMultiScale(eq_img, faces, 1.3, 5);

        // Reset total x & y values
        total_x  = 0;
        total_y  = 0;

        // Calc total x & y values for face positions and draw rect around faces.
        for (i = faces.begin(); i!= faces.end(); ++i) {
            total_x += i->x+i->width/2;
            total_y += i->y+i->height/2;
                rectangle(
                    src_img,
                    Point(i->x,i->y),
                    Point(i->x + i->width, i->y + i->height),
                    CV_RGB(255,0,0),
                    2);
            }

        // If any faces is found
        if (faces.size())
        {
            rounds++;

            // Calc average x & y value for faces
            average_x = total_x / faces.size();
            average_y = total_y / faces.size();

            if (average_x > WIDTH / 2 + STOPAREA)
            {
                rounds_ac++;
                rounds_di++;
            }
            else if (average_x < WIDTH / 2 - STOPAREA)
                rounds_ac++;
        }

        // If all rounds suggested to activate the motor
        if (rounds_ac == CHECKROUNDS)
        {
            firmata_digitalWrite(firmata, ACPIN, HIGH);

            // If all rounds suggested to go right
            if (rounds_di == CHECKROUNDS)
            {
                firmata_digitalWrite(firmata, DIPIN, HIGH);
                usleep(TIME);
                firmata_digitalWrite(firmata, ACPIN, LOW);
                cout << "Go right" <<  endl;
            }
            // If all rounds suggested to go left
            else if (rounds_di == 0)
            {
                firmata_digitalWrite(firmata, DIPIN, LOW);
                usleep(TIME);
                firmata_digitalWrite(firmata, ACPIN, LOW);
                cout << "Go left" <<  endl;
            }
        }
        // If any rounds suggested to stop the motor
        else
        {
            firmata_digitalWrite(firmata, ACPIN, LOW);
            cout << "Stay" <<  endl;
        }

        // Reset rounds if CHECKROUNDS is reached.
        if (rounds == CHECKROUNDS) {
            rounds_ac = 0;
            rounds_di = 0;

            rounds = 0;
        }

        // Show image
        imshow("track-tv", src_img);

        // Activate pneumatic cylinder
        if (key == KEY_P) {
            pnstate = !pnstate;
            firmata_digitalWrite(firmata, PNPIN, pnstate);
        }
    }
    while (key != KEY_ESC);

    // Stop motor
    firmata_digitalWrite(firmata, ACPIN, LOW);

    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
