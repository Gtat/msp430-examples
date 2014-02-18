import collections
import itertools

import serial
import struct
import crcmod

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
    crc = crcmod.mkCrcFun(0x107)

    while True:
      buf.appendleft(ser.read())

      if len(buf) >= BYTES_PER_INCOMING_PACKET:
        packet = []
        for i in xrange(BYTES_PER_INCOMING_PACKET):
          x = buf.pop()
          print '%02x ' % ord(x),
          packet.append(x)          
        check = crc(''.join(packet[:-1]))
        if ord(packet[-1]) != check:
          print 'CRC FAIL: %02x vs %02x' % (ord(packet[-1]), check)
          buf.extendleft(packet[1:])
        else:
          print 'CRC OK:   %02x' % check

  except KeyboardInterrupt:
    print '^C'
    return
  

if __name__ == '__main__':
  main('/dev/ttyACM0')
