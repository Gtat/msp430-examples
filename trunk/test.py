# @file    test.py
# @author  Sam Boling <charles.samuel.boling@gmail.com>
# @date    January-August 2014
# @version 0.1
# @section DESCRIPTION
# Script for minimal interaction with the microcontroller software.
# Includes CRC checking, start and halt capture commands, and lets
# commands be sent incrementally as dictated by the user.

import sys
import collections
import itertools

import serial
import struct
import crcmod

# pulls in a routine to construct any supported packet type by name
sys.path.append("../../gui/PyModules")
import comm.packets

BYTES_PER_INCOMING_PACKET = 8

def send_packet(port, checker, *args, **kwargs):
  '''
  Transmit a packet to the microcontroller.
  Print the raw contents of the packet.
    port    -- an open serial.Serial object for RS232 communication
    checker -- a crcmod CRC-generating function
    args, kwargs -- packet name followed by parameters, 
                    see ../../gui/PyModules/comm/packets.py for details
  '''
  packet = comm.packets.build_downlink_packet(checker, *args, **kwargs)
  for x in packet:
    print '%02x ' % ord(x),
  print  
  port.write(packet)

def parse_packet(ser, crc, buf):
  '''
  Receive and unpack an uplink packet, discarding it if the CRC fails.
  Print its contents and CRC outcome.

  ser -- an open serial.Serial object for RS232 communication
  crc -- a crcmod CRC-generating function
  buf -- a collections.Deque to be used as a circular buffer
  '''

  # rotate a new byte into the buffer
  buf.appendleft(ser.read())
  if len(buf) >= BYTES_PER_INCOMING_PACKET:
    packet = []
    for i in xrange(BYTES_PER_INCOMING_PACKET):
      x = buf.pop()
      print '%02x ' % ord(x),
      packet.append(x)          

    check = crc(''.join(packet[:-1]))
    if ord(packet[-1]) != check:
      print '\t\tCRC FAIL: %02x vs %02x' % (ord(packet[-1]), check)
      # skip a byte, attempting to restore 
      # synchronization on the next try.
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
#    send_packet(ser, crc, "SET_RATES", taccr=0x1999)

    raw_input()
    send_packet(ser, crc, "CAPTURE")
#    raw_input()
#    send_packet(ser, crc, "DUMP")

    while True:
      parse_packet(ser, crc, buf)

  except KeyboardInterrupt:
    print '^C'

  send_packet(ser, crc, "HALT")

#  send_packet(ser, crc, "DUMP")

  try:
    while True:
      parse_packet(ser, buf, crc)
  except KeyboardInterrupt:
    print '^C'

if __name__ == '__main__':
  main('/dev/ttyACM0')
