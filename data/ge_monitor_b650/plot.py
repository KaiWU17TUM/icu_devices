import numpy as np
import math
import matplotlib.pyplot as plt

data = np.genfromtxt("Wed_Jul _2021_DRI_WF_ECG1.csv", delimiter=",", names=["x", "y","z"])
Y = data['y']
Y2 = []
for y in Y:
    if not math.isnan(y):
        Y2.append(y)

plt.plot(Y2)
plt.show()
