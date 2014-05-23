#!/usr/bin/python
import cv2
import pyfirmata

# OpenCV
cam = cv2.VideoCapture(0)
face_cascade = cv2.CascadeClassifier('haarcascade_frontalface_default.xml')
cv2.namedWindow("track-tv", cv2.WINDOW_AUTOSIZE)

height, width, depth = cam.read()[1].shape
centerX=width/2
print height,width

# pyfirmata
board = pyfirmata.Arduino('/dev/ttyACM99')
it = pyfirmata.util.Iterator(board)
it.start()

# Enable Analog pins
board.analog[0].enable_reporting()
board.analog[1].enable_reporting()

#board.pass_time(1)
#print(board.analog[0].read())
HIGH,LOW  = range(2)
mes1,mes2 = [],[]

# Vars
stArea = 20
reqMes = 3

# Pins
diPin = 2
moPin = 3

while 0>cv2.waitKey(1):

    # Capture image
    img = cam.read()[1]
    
    # Convert image to gray to improve cascade analysis
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    
    # Fix color
    eq = cv2.equalizeHist(gray)
    
    # Detect faces with cascade
    faces = face_cascade.detectMultiScale(eq, 1.3, 5)
    
    # Calculate average position
    avX = 0
    avY = 0
    for (x,y,w,h) in faces:
        cv2.rectangle(img,(x,y),(x+w,y+h),(255,0,0),2)
        avX += x+w/2
        avY += y+h/2
    
    if len(faces)>0:
	avX = avX/len(faces)
	avY = avY/len(faces)

	if avX>centerX+stArea:
	    #print "Go right"
	    mes1.append(HIGH)
	    mes2.append(HIGH)
	    #board.digital[diPin].write(1)
	    #board.digital[moPin].write(1)
	elif avX<centerX-stArea:
	    #print "Go left"
	    mes1.append(HIGH)
	    mes2.append(LOW)
	    #board.digital[diPin].write(0)
	    #board.digital[moPin].write(1)
	else:
	    #print "Stay"
	    mes1.append(LOW)
	    #board.digital[moPin].write(0)
    else:
	mes1.append(LOW)
	#board.digital[moPin].write(0)
	
    if len(mes1) == reqMes:
	if mes1.count(HIGH) == len(mes1):
	    print "Start motor"
	    board.digital[moPin].write(1)
	else:
	    print "Stop motor"
	    board.digital[moPin].write(0)
	mes1=[]
    
    if len(mes2) == reqMes:
	if mes2.count(HIGH)==len(mes2):
	    print "Go right"
	    board.digital[diPin].write(0)
	elif mes2.count(LOW)==len(mes2):
	    print "Go left"
	    board.digital[diPin].write(1)
	mes2=[]
	

    cv2.imshow("track-tv", img)

cv2.destroyWindow("track-tv")
board.exit()
