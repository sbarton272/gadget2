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

import sys
import accelerometer as accel

PORT = 6 # COM 7
BAUD = 9600
TIMEOUT = 3 # sec
MAXLINE = 32
SAMPLES = 1000
THRESHHOLD = .2
ABC = (.33, .33, .33)


def safeInitGadget( baud, timeout, port, threshHold, (a,b,c) ):
	try:
		gadget = accel.ACCELEROMETER( baud, timeout, port, threshHold, (a,b,c) );
		return gadget
	except Exception as E:
		print
		print 'Error: ', E
		sys.exit(1)


def main():
	
	print "Starting HALT Laptop Security"
	
	# Handler for not connected
	gadget = safeInitGadget( BAUD, TIMEOUT, PORT, THRESHHOLD, ABC )
	
	alarm = accel.ALARM();

	gadget.blinkLED(1)

	i = 0
	try: # handler for disconnect

		while i < SAMPLES:
			gadget.ser.write("x") # request data

			read = gadget.ser.readline(MAXLINE)

			if read != "\n":
				(X,Y,Z) = gadget.parseXYZ(read)

				if gadget.motionDetected():
					print "Motion"
					alarm.soundSiren()
					break # password correct, so program over

				i += 1

		gadget.closeConn()
		gadget.dataWrite("xyzData.txt")

	except Exception as E:
		# if gadget disconnected trigger alarm, can tell if read fails
		try:
			gadget.ser.read()
			print E
			sys.exit(1) # some other error
		except:
			print "Caught you, nice try at disconnecting"
			alarm.soundSiren()

if __name__ == "__main__":
	main()