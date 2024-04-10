from time import sleep
import serial
import serial.tools
import struct
import serial.tools.list_ports

DEV = '/dev/ttyACM1'

def findDevice():
    ports = serial.tools.list_ports.comports()
    for p in ports:
        print(p)
        print(p.usb_info())
        print(p.usb_description())

def main():
    state = False
    findDevice()
    interface = serial.Serial(DEV, 115200)
    
    while True:
        if state:
            print('On')
            buff = struct.pack('<BB', 0,0)
            interface.write(buff)
            interface.read_all()
        else:
            print('Off')
            buff = struct.pack('<BB', 1,0)
            interface.write(buff)
            interface.read_all()
            
        state = not state
        sleep(1)
        

if __name__ == '__main__':
    main()