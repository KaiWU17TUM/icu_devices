import numpy as np
import math
import matplotlib.pyplot as plt

data = np.genfromtxt("Thu__Jun_2021_RealtimeData.csv", delimiter=",", names=["x", "y"])
Y = data['y']
Y2 = []
for y in Y:
    if not math.isnan(y):
        Y2.append(y)

plt.plot(Y2)
plt.show()