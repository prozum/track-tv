#!/usr/bin/python
import cv2

from drivers.firmata import *

# Vars
stArea = 20
reqMes = 3

# Pins
diPin = 2
moPin = 3

class Application:
    def setup(self):
        # OpenCV
        self.cam = cv2.VideoCapture(0)
        self.face_cascade = cv2.CascadeClassifier('haarcascade_frontalface_default.xml')
        cv2.namedWindow("track-tv", cv2.WINDOW_AUTOSIZE)

        height, width, depth = self.cam.read()[1].shape
        self.centerX = width/2

        # Lists for measured data
        self.mes1,self.mes2 = [],[]

    def loop(self):
        # Capture image
        img = self.cam.read()[1]
        
        # Convert image to gray to improve cascade analysis
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        
        # Fix color
        eq = cv2.equalizeHist(gray)
        
        # Detect faces with cascade
        faces = self.face_cascade.detectMultiScale(eq, 1.3, 5)
        
        # Calculate average position
        avX = 0
        avY = 0
        for (x,y,w,h) in faces:
            cv2.rectangle(img,(x,y),(x+w,y+h),(0,0,255),2)
            avX += x+w/2
            avY += y+h/2
        
        if len(faces)>0:
            avX = avX/len(faces)
            avY = avY/len(faces)

            if avX>self.centerX+stArea:
                self.mes1.append(HIGH)
                self.mes2.append(HIGH)
            elif avX<self.centerX-stArea:
                self.mes1.append(HIGH)
                self.mes2.append(LOW)
            else:
                self.mes1.append(LOW)

        else:
            self.mes1.append(LOW)
            
        if len(self.mes1) == reqMes:
            if self.mes1.count(HIGH) == len(self.mes1):
                serialPrintln("Start motor")
                #self.board.digital[moPin].write(HIGH)
                digitalWrite(moPin,HIGH)
            else:
                serialPrintln("Stop motor")
                #self.board.digital[moPin].write(LOW)
                digitalWrite(moPin,LOW)
            self.mes1=[]
        
        if len(self.mes2) == reqMes:
            if self.mes2.count(HIGH)==len(self.mes2):
                serialPrintln("Go right")
                #self.board.digital[diPin].write(LOW)
                digitalWrite(diPin,LOW)
            elif self.mes2.count(LOW)==len(self.mes2):
                serialPrintln("Go left")
                #self.board.digital[diPin].write(HIGH)
                digitalWrite(diPin,HIGH)
            self.mes2=[]
            

        cv2.imshow("track-tv", img)

if __name__ == '__main__':
    driver = Driver("/dev/ttyACM0")
    digitalWrite  = driver.digitalWrite
    serialPrintln = driver.serialPrintln
    
    app = Application()
    app.setup()
    while 0>cv2.waitKey(1):
        app.loop()
    
    cv2.destroyWindow("track-tv")
    driver.exit()
