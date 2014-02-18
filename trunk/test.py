import collections
import serial
import struct

BYTES_PER_INCOMING_PACKET = 8

def main(serial_path):
  try:
    ser = serial.Serial()
    ser.port     = serial_path
    ser.baudrate = 9600
 
    ser.open()
    if ser.isOpen():
      print '%s open' % ser.name
    else:
      return

    buf = collections.deque()

    while True:
      bytect = 0
      while bytect < BYTES_PER_INCOMING_PACKET:
        b = ser.read()
        print '%02x ' % ord(b),
        bytect += 1
      print '\n',

  except KeyboardInterrupt:
    print '^C'
    return
  

if __name__ == '__main__':
  main('/dev/ttyACM0')
