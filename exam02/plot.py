import matplotlib.pyplot as plt
import numpy as np
import serial
import time

x = np.arange(0, 10, 0.1)
y = np.arange(0, 10, 0.1)
z = np.arange(0, 10, 0.1)
log = np.arange(0, 10, 0.1)
t = np.arange(0, 10, 0.1)

serdev = '/dev/ttyACM0'
s = serial.Serial(serdev, 115200)
for i in range(0, 100):
   for j in range(0, 4):
       line=s.readline() # Read an echo string from K66F terminated with '\n'
       print(line)
       if j == 0:
          log[i] = int(line)
       elif j == 1:
          x[i] = float(line)
       elif(j == 2):
          y[i] = float(line)
       else:
          z[i] = float(line)


fig, ax = plt.subplots(2, 1)
ax[0].plot(t,x, 'b', label = 'x', color = 'b')
ax[0].plot(t,y, 'r', label = 'y', color = 'r')
ax[0].plot(t,z, 'g', label = 'z', color = 'g')
ax[0].set_xlabel('Time')
ax[0].set_ylabel('Acc vector')
ax[0].legend()
ax[1].stem(t,log)
ax[1].set_xlabel('time')
ax[1].set_ylabel('logger')
plt.show()
s.close()