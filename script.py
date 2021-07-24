import serial
import time
import subprocess

try:
    ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
    ser.flush()
except serial.SerialException:
    print('SERIAL_ERROR')
    exit(2)

time.sleep(2)

while(1):
    command = ser.readline().decode('utf-8').rstrip()
    if command:
        print(command)
    else:
        time.sleep(1)
    if(command == 'REVERSE_ENGAGED'):
        # switch to camera
        subprocess.call('echo 1 > /sys/class/gpio/gpio20/active_low', shell=True)
    elif(command == 'REVERSE_DISENGAGED'):
        # switch back to OpenAuto
        subprocess.call('echo 0 > /sys/class/gpio/gpio20/active_low', shell=True)
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

    #time.sleep(1)

