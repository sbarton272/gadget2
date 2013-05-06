
# 16-250 S2013
# gadget 2
# sebarton

# read from serial, analyze and active alarm
# serial from http://pyserial.sourceforge.net/

import serial, time, re

port = 6 # COM 7
baud = 9600
timeout = 3 # sec

class ACCELEROMETER(object):
	
	def __init__(self):
		self.ser = self.initConn()
		self.XYZ = []

	def X(self, time):
		return self.XYZ[time][0]

	def Y(self, time):
		return self.XYZ[time][1]

	def Z(self, time):
		return self.XYZ[time][3]

	def blinkLED(self, delay):
		print "Blinking LED with delay of %d sec" % delay
		self.ser.write("o")
		time.sleep(delay)
		self.ser.write("l")

	def parseXYZ(self, data):
		# format like     X: 0x3d; Y: 0xd; Z: 0x15;
		regex = re.compile("X: (0x[0-9a-fA-F]{1,2}); Y: (0x[0-9a-fA-F]{1,2}); Z: (0x[0-9a-fA-F]{1,2});")
		r = regex.search(data)
		if r:
			matched = r.groups()

			# convert from hex to dec
			X = int( matched[0], 16 )
			Y = int( matched[1], 16 )
			Z = int( matched[2], 16 )

			self.XYZ.append( (X,Y,Z) )
			return (X,Y,Z)

		# error, nothing matched
		return data


	def initConn(self):
		ser = serial.Serial()
		ser.baudrate = baud
		ser.port = port
		ser.timeout = timeout
		print "Stats\n", ser

		ser.open()
		print "Opened? ", ser.isOpen()
		print
		return ser

	def closeConn(self):
		self.ser.close()
		print "Closed? ", (not self.ser.isOpen())
		print


def main():
	
	print "Starting HALT Laptop Security"
	
	gadget = ACCELEROMETER();

	gadget.blinkLED(1)

	for i in xrange(1,1000):
		gadget.ser.write("x") # request data

		read = gadget.ser.readline(26)
		if read != "\n":
			print gadget.parseXYZ(read)
	
	gadget.closeConn()


if __name__ == "__main__":
	main()