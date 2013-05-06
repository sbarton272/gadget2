
# 16-250 S2013
# gadget 2
# sebarton

# read from serial, analyze and active alarm
# serial from http://pyserial.sourceforge.net/

import serial, time, re, sys
import lookupTable as lookup

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

	def parseAxis(self, data, type):
		data_dec = int( data, 16 )

		alert = (data_dec >> 6) & 1 # alert is bit 6
		
		measurment = data_dec & 63 # data is in bottom 6 bits

		shaken = measurment in lookup.shaken

		(force, angleXY, angleZ) = lookup.lookupXYZ.get(measurment)

		print shaken, force, angleXY, angleZ
		return measurment



	def parseXYZ(self, data):
		# format like     X: 0x3d; Y: 0xd; Z: 0x15;
		regex = re.compile("X: (0x[0-9a-fA-F]{1,2}); Y: (0x[0-9a-fA-F]{1,2}); Z: (0x[0-9a-fA-F]{1,2});")
		r = regex.search(data)
		if r:
			matched = r.groups()

			# convert from hex to dec
			X = self.parseAxis( matched[0], "X")
			Y = self.parseAxis( matched[1], "Y")
			Z = self.parseAxis( matched[2], "Z")

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

	while 1:
		gadget.ser.write("x") # request data

		read = gadget.ser.readline(26)
		if read != "\n":
			(X,Y,Z) = gadget.parseXYZ(read)
			print (X,Y,Z)
			#print "X: ", "="*X
			#print "Y: ", "="*Y
			#print "Z: ", "="*Z

	gadget.closeConn()


if __name__ == "__main__":
	main()