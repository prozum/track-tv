#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>

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

#define WIDTH    	640
#define HEIGHT   	480
#define STOPAREA 	40
#define CHECKROUNDS   	3
#define TIME		200000

int main(int, char *argv[])
{
    t_firmata *firmata;
    firmata = firmata_new( (char*) "/dev/ttyACM99"); //init Firmata
    
    while(!firmata->isReady) //Wait until device is up
	firmata_pull(firmata);
    
    firmata_pinMode(firmata, DIPIN, MODE_OUTPUT); // Direction pin
    firmata_pinMode(firmata, ACPIN, MODE_OUTPUT); // Activate pin
    firmata_pinMode(firmata, PNPIN, MODE_OUTPUT); // Pn pin
  
    namedWindow("track-tv",WINDOW_NORMAL);
    setNumThreads(4);  // 4 theads for TBB
    cvUseOptimized(1); // Use optimizations
  
    VideoCapture cam(0); // open the default camera
    if(!cam.isOpened()) {
	cout << "Cannot open cam!" << endl;
        return -1;
    }

    CascadeClassifier cas;
    if  (!cas.load(string(argv[1]))) {
        cout << "Cannot read cascade!" << endl;
        return -1;
    }

    Mat src_img, gray_img, eq_img;
    
    vector<Rect> faces;
    vector<Rect>::const_iterator i;

    int mes[2];
    int av[2];
    int rounds = 0;
    int key = -1;
    bool pnstate = false; 
    
    do
    {
        key = waitKey(1);

        av[0]  = 0;
        av[1]  = 0;

        cam >> src_img; // get a new frame from camera

        cvtColor(src_img, gray_img, CV_RGB2GRAY);
        equalizeHist(gray_img,eq_img);

        cas.detectMultiScale(eq_img,faces,1.3,5);

        for (i = faces.begin(); i!= faces.end(); ++i) {
            av[0] += i->x+i->width/2;
            av[1] += i->y+i->height/2;
                rectangle(
                    src_img,
                    Point(i->x,i->y),
                    Point(i->x + i->width, i->y + i->height),
                    CV_RGB(255,0,0),
                    2);
            }

        if (faces.size()>0)
        {
            rounds++;

            av[0] = av[0]/faces.size();
            av[1] = av[1]/faces.size();

            if (av[0]>WIDTH/2+STOPAREA)
            {
            mes[0]++;
            mes[1]++;
            }
            else if (av[0]<WIDTH/2-STOPAREA)
            {
            mes[0]++;
            }
        }

        if (mes[0]==CHECKROUNDS) // If active motor
        {
            if (mes[1]==CHECKROUNDS) // If right
            {
                firmata_digitalWrite(firmata, ACPIN, HIGH);
                firmata_digitalWrite(firmata, DIPIN, HIGH);
                usleep(TIME);
                firmata_digitalWrite(firmata, ACPIN, LOW);
                cout << "Go right" <<  endl;
            }
            else if (mes[1]==0) // If left
            {
                firmata_digitalWrite(firmata, ACPIN, HIGH);
                firmata_digitalWrite(firmata, DIPIN, LOW);
                usleep(TIME);
                firmata_digitalWrite(firmata, ACPIN, LOW);
                cout << "Go left" <<  endl;
            }
        }
        else
        {
            firmata_digitalWrite(firmata, ACPIN, LOW);
            cout << "Stay" <<  endl;
        }

        if (rounds==CHECKROUNDS)  {
            mes[0] = 0;
            mes[1] = 0;

            rounds = 0;
        }
	
        imshow("track-tv", src_img);
	
        if (key==116) {
            pnstate = !pnstate;
            firmata_digitalWrite(firmata, PNPIN, pnstate);
        }
    }
    while (key!=27);
    
    firmata_digitalWrite(firmata, ACPIN, LOW);

    // the camera will be deinitialized automatically in VideoCapture destructor
    return 0;
}
