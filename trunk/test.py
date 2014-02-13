import serial
import struct

def main(serial_path):
  try:
    ser = serial.Serial(serial_path, 9600)
    print ser.name

    bytect = 0
    while bytect < 8:
      b = ser.read()
      print '%s ' % hex(ord(b))
      bytect += 1
  except KeyboardInterrupt:
    print '^C'
    return
  

if __name__ == '__main__':
  main('/dev/ttyACM0')
