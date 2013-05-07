

import serial, time, re, sys, os, pyaudio, wave, winsound, time
from select import select
from multiprocessing import Pool
import lookupTable as lookup

CHUNK = 1024

class ACCELEROMETER(object):
	
	def __init__(self, baud, timeout, port, threshHold, abc):
		self.XYZ = [] # elems: (X, Y, Z), each axis (shaken, force, angle)
		self.abc = abc # (a, b, c)
		self.ser = self.initConn( baud, timeout, port)
		self.threshHold = threshHold
		self.sumXg = 0 # running sum of X g values

	def X(self, time):
		return self.XYZ[time][0]

	def Y(self, time):
		return self.XYZ[time][1]

	def Z(self, time):
		return self.XYZ[time][3]

	# X force specific functions as using this data for analysis
	# @TODO make axis object, generalize these functions

	def Xforce(self, time):
		return self.XYZ[time][0][1]

	def smoothXforce(self, index):
		# smooth X g data
		# y[n] = a*x[n]+b*x[n-1]+c*x[n-2]
		(a, b, c) = self.abc

		if index < 0:
			return 0
		elif index == 0:
			return self.Xforce(0)*a
		elif index == 1:
			return self.Xforce(1)*a + self.Xforce(0)*b
		else:
			return self.Xforce(index)*a + self.Xforce(index-1)*b + self.Xforce(index-2)*c

	def motionDetected(self):
		# look for spike in measurments, signalling movement
		end = len(self.XYZ)
		average = self.sumXg / end
		Xval = self.smoothXforce(end - 1) # look at last element and prior element
		XvalPrior = self.smoothXforce(end - 1) # look at last element and prior element
		dif1 = abs( Xval - average )
		dif2 = abs( XvalPrior - average )
		dif = (dif1 + dif2) / 2 # average last two values to smooth data

		# @TODO calculate running sd: http://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
		if ( dif > self.threshHold ):
			return True
		return False

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

			# store X running sum
			self.sumXg += X[1] # running sum of X g values

			return (X,Y,Z)

		# error, nothing matched
		return data

	def initConn(self, baud, timeout, port):
		ser = serial.Serial()
		ser.baudrate = baud
		ser.port = port
		ser.timeout = timeout
		print "Stats\n", ser

		try:
			ser.open()
			print "Opened? ", ser.isOpen()
			print
			return ser
		except:
			raise NameError("Gadget not connected")

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



############################################################################


class ALARM(object):

	def __init__(self, password = "STOP", sirenFile = "sounds/siren.wav", duration = 10000):
		self.duration = duration
		self.sirenFile = sirenFile
		self.password = password
		self.siren = wave.open(self.sirenFile, 'rb')

	def soundAlarm( self ):
		winsound.Beep(4400, self.duration)

	def soundSiren( self ):
		print "Alarm set"	
		
		self.playSiren()

		while 1:
			# wait for correct password
			if self.getPassword():
				break

		self.stopSiren()

	def playSiren( self ):
		flags = winsound.SND_FILENAME | winsound.SND_ASYNC | winsound.SND_LOOP
		winsound.PlaySound(self.sirenFile, flags)

	def stopSiren( self ):
		winsound.PlaySound(None, 0)

	def getPassword( self ):

		print "Enter password:",
		password = sys.stdin.readline().strip()

		if password == self.password:
			print "Alarm done :)"
			return True
		else:
			print "Nice try :)"
			return False