#!/usr/bin/env python

import serial
import time

import matplotlib.pyplot as plt
import numpy as np

def grb_to_color(grb):
    pass

# get the data
ser = serial.Serial('/dev/ttyACM0')
ser.baudrate = 115200
ser.flushInput()

time.sleep(3) # startup time

ser.write(b'c')
time.sleep(1)

ser.write(b'y')

nrows, ncols = 240, 320
image = np.zeros(nrows*ncols*3)
image = image.reshape((nrows, ncols, 3))

idx = 0
ri = 0
ci = 0

while True:
    try:
        ser_bytes = ser.readline()
        decoded = str(ser_bytes[0:len(ser_bytes)-1].decode("utf-8")).strip()
        # print "decoded:", decoded
        if "PIND value " in decoded:
            b_str = decoded.replace("PIND value ", "")
            # b_str = b_str[:5] # just the green bits
            # v = int(b_str, 2) # G4:R2:B2

            # image[idx] = v # (0, 0, 255)
            # idx += 1
            # print "idx:", idx

            image[ri, ci, 0] = int(b_str[:5], 2)
            image[ri, ci, 1] = int(b_str[5:11], 2)
            image[ri, ci, 2] = int(b_str[11:], 2)

            ci = (ci + 1) % ncols
            if (ci == 0):
                ri = (ri + 1) % nrows
        elif decoded == "done":
            print "done", ri
            break
    except:
        print "interrupt"
        break

# image = image.reshape((nrows, ncols))

plt.imshow(image)
plt.show()