
# 16-250 S2013
# gadget 2
# sebarton

# read from serial, analyze and active alarm

import serial, time

port = 6
baud = 9600

def main():
	
	print "Starting HALT Laptop Security"

	ser = serial.Serial()
	ser.baudrate = baud
	ser.port = port
	print "Stats\n", ser

	ser.open()
	print "Opened? ", ser.isOpen()
	
	ser.write("h")
	print ser.read(1)
	ser.write("o")
	time.sleep(1)
	ser.write("l")

	ser
	ser.close()


if __name__ == "__main__":
	main()