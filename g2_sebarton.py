
# 16-250 S2013
# gadget 2
# sebarton

# read from serial, analyze and active alarm
# serial from http://pyserial.sourceforge.net/

import serial, time, re, sys, winsound
import lookupTable as lookup

port = 6 # COM 7
baud = 9600
timeout = 3 # sec
MAXLINE = 32
(a, b, c) = (.2, .4, .4)

class ALARM(object):

	def soundAlarm( self, duration = 10000):
		winsound.Beep(4400, duration)

class ACCELEROMETER(object):
	
	def __init__(self, abc):
		self.ser = self.initConn()
		self.XYZ = []
		self.abc = abc # (a, b, c)

	def X(self, time):
		return self.XYZ[time][0]

	def Y(self, time):
		return self.XYZ[time][1]

	def Z(self, time):
		return self.XYZ[time][3]

	def diff(self, time):
		return self.XYZ[time][4]

	def getDiff(self, data):
		# y[n] = a*x[n]+b*x[n-1]+c*x[n-3]
		(a, b, c) = self.abc
		time = len(data) # @TODO
		return 

	def blinkLED(self, delay):
		print "Blinking LED with delay of %d sec" % delay
		self.ser.write("o")
		time.sleep(delay)
		self.ser.write("l")

	def parseAxis(self, data, axis):
		data_dec = int( data, 16 )

		alert = (data_dec >> 6) & 1 # alert is bit 6
		
		measurment = data_dec & 63 # data is in bottom 6 bits

		shaken = measurment in lookup.shaken

		(force, angleXY, angleZ) = lookup.lookupXYZ.get(measurment)

		# return proper angle
		if axis == "X" or axis == "Y":
			return (shaken, force, angleXY)
		elif axis == "Z":
			return (shaken, force, angleZ)
		else:
			return None


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

	def dataWrite(self, filename):
		fout = open( filename , "w")

		# header
		header = "X shaken\tX g's\tX angle\t" 
		header += "Y shaken\tY g's\tY angle\t" 
		header += "Z shaken\tZ g's\tZ angle\t" + "\n"
		fout.write( header )

		for line in self.XYZ:
			X = line[0]
			Y = line[1]
			Z = line[2]
			formatted = "%r\t%r\t%r\t%r\t%r\t%r\t%r\t%r\t%r\n" % ( X[0], X[1], X[2], Y[0], Y[1], Y[2], Z[0], Z[1], Z[2])

			# replace None with empty
			formatted = formatted.replace("None","")

			fout.write( formatted )

		fout.close()
		print "Data written to ", filename


def main():
	
	print "Starting HALT Laptop Security"
	
	gadget = ACCELEROMETER( (a,b,c) );
	alarm = ALARM();

	gadget.blinkLED(1)


	i = 0
	while i < 100:
		gadget.ser.write("x") # request data

		read = gadget.ser.readline(MAXLINE)

		if read != "\n":
			(X,Y,Z) = gadget.parseXYZ(read)

			#if gadget.motionDetected():
			#	print "MOTION"
			
			i += 1

	gadget.closeConn()
	gadget.dataWrite("xyzData.txt")

	alarm.soundAlarm()

if __name__ == "__main__":
	main()