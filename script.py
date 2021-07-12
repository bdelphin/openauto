import serial
import time
import subprocess

try:
    ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
    ser.flush()
except serial.SerialException:
    print('SERIAL_ERROR')
    exit(2)

time.sleep(2)

while(1):
    command = ser.readline().decode('utf-8').rstrip()
    if(command == 'REVERSE_ENGAGED'):
        # switch to camera
        print('reverse')
    elif(command == 'REVERSE_DISENGAGED'):
        # switch back to OpenAuto
        print('normal')
    elif(command == 'VOL_UP'):
        subprocess.call(["xdotool", "key", "F8"])    
    elif(command == 'VOL_DOWN'):
        subprocess.call(["xdotool", "key", "F7"])
    elif(command == 'PLAY'):
        subprocess.call(["xdotool", "key", "X"])
    elif(command == 'PAUSE'):
        subprocess.call(["xdotool", "key", "C"])
    elif(command == 'NEXT'):
        subprocess.call(["xdotool", "key", "N"])

    time.sleep(1)

