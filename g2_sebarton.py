#
# 16-250 S2013
# gadget 2
# sebarton
#
# read from serial, analyze and active alarm
# serial from http://pyserial.sourceforge.net/
# Uses pyaudio: http://people.csail.mit.edu/hubert/pyaudio/docs/
#
# Running python 2.7.3
#
# @TODO
#


import accelerometer as accel

port = 6 # COM 7
baud = 9600
timeout = 3 # sec
MAXLINE = 32
SAMPLES = 1000
threshHold = .2
(a, b, c) = (.33, .33, .33)


def main():
	
	print "Starting HALT Laptop Security"
	
	gadget = accel.ACCELEROMETER( baud, timeout, port, threshHold, (a,b,c) );
	alarm = accel.ALARM();

	gadget.blinkLED(1)


	i = 0
	while i < SAMPLES:
		gadget.ser.write("x") # request data

		read = gadget.ser.readline(MAXLINE)

		if read != "\n":
			(X,Y,Z) = gadget.parseXYZ(read)

			if gadget.motionDetected():
				print "Motion"
				alarm.soundSiren()
				
			
			i += 1

	gadget.closeConn()
	gadget.dataWrite("xyzData.txt")

if __name__ == "__main__":
	main()