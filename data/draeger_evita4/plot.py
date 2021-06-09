import numpy as np
import matplotlib.pyplot as plt

data = np.genfromtxt("Wed_Jun _2021_RealtimeData.csv", delimiter=",", names=["x", "y"])
plt.plot(data['y'])
plt.show()