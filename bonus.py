import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from sklearn.datasets import load_iris
from sklearn.cluster import KMeans
dataset = load_iris()
X = dataset.data
Y = dataset.target

lst = []
for i in range(1,11):
    Kmeans = KMeans(n_clusters = i, init = 'k-means++', max_iter = 200, \
                    n_init = 10, random_state = 0)
    Kmeans.fit(X, Y)
    lst.append(Kmeans.inertia_)

plt.plot(range(1,11), lst)
ax = plt.gca()
plt.title("Elbow Method for selection of optimal K clusters")
plt.xlabel("K")
plt.ylabel("Average Dispersion")
ax.arrow(4.35, 200, -1, -80, width=0.1, head_length=20, fc='k', ec='k')


plt.scatter(3, 83, s=80, facecolors='none', edgecolors='k')
plt.savefig('elbow.png')
