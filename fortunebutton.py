# Sleep for 3 seconds, then put into push mode, wait for a button press,
# then put back into sweep mode

import serial, time, sys

if len(sys.argv) != 2:
    print("%s <serial port>" % sys.argv[0], file=sys.stderr)
    sys.exit(1)

ser = serial.Serial(sys.argv[1], 115200)

ser.write(bytes([1]))
ser.flush()

button = ser.read(1)[0]
print("button %d was pressed" % button)

time.sleep(2)

ser.write(bytes([0]))
ser.flush()

ser.close()
