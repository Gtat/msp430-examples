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

def parse_packet(ser, buf, crc):
  buf.appendleft(ser.read())

  if len(buf) >= BYTES_PER_INCOMING_PACKET:
    packet = []
    for i in xrange(BYTES_PER_INCOMING_PACKET):
      x = buf.pop()
      print '%02x ' % ord(x),
      packet.append(x)          

    # human-readable voltages
#    for x in packet[1:-2]:
#      print '%0.03f' % ((ord(x) / 255.0) * 3.5),

    check = crc(''.join(packet[:-1]))
    if ord(packet[-1]) != check:
      print '\t\tCRC FAIL: %02x vs %02x' % (ord(packet[-1]), check)
      buf.extendleft(packet[1:])
    else:
      print '\t\tCRC OK:   %02x' % check,
      print ' ID: %02x' % ord(packet[0])

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
#    raw_input()
#    send_packet(ser, crc, 0x05, 0x99, 0x19)

    # CAPTURE message
#    raw_input()
#    send_packet(ser, crc, 0x02, 0, 0)
    # DUMP message
    raw_input()
    send_packet(ser, crc, 0x01, 0, 0)

    while True:
      parse_packet(ser, buf, crc)

  except KeyboardInterrupt:
    print '^C'
    # HALT message
    send_packet(ser, crc, 0x03, 0, 0)
    return
  

if __name__ == '__main__':
  main('/dev/ttyACM0')
