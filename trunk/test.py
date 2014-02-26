import collections
import itertools

import serial
import struct
import crcmod

BYTES_PER_INCOMING_PACKET = 8

def send_packet(port, checker, *args):
  packet = struct.pack('BBB', *args)
  check  = struct.pack('B', checker(packet))
  print 'SEND ',
  for x in (packet + check):
    print '%02x ' % ord(x),
  print
  port.write(packet + check)

def parse_packet(ser):
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

def main(serial_path):
  buf = collections.deque()
  crc = crcmod.mkCrcFun(0x107)
    
  ser = serial.Serial()
  ser.port     = serial_path
  ser.baudrate = 9600
 
  ser.open()
  if ser.isOpen():
    print '%s open' % ser.name
    print 'Hit enter to send the next packet'
    print
  else:
    return

  try:
    # SET_RATES to 2 Hz
    raw_input()
    send_packet(ser, crc, 0x50, 0xb0, 0x28)

    # CAPTURE message
    raw_input()
    send_packet(ser, crc, 0x20, 0, 0)

    while True:
      print hex(ord(ser.read(1)))
      #parse_packet(ser)

  except KeyboardInterrupt:
    print '^C'
    # HALT message
    send_packet(ser, crc, 0x30, 0, 0)
    return
  

if __name__ == '__main__':
  main('/dev/ttyACM0')
