import numpy as np

def lagrange(x_nodes, y_nodes, x):

    x_nodes = np.asarray(x_nodes, dtype=float)
    y_nodes = np.asarray(y_nodes, dtype=float)
    x = np.asarray(x, dtype=float)
    y = np.zeros_like(x, dtype=float)
    for i in range(len(y_nodes)):
        divider = 1
        for j in range(len(x_nodes)):
            if j!=i:
                divider *= x_nodes[i] - x_nodes[j]

        numerator = np.ones_like(x, dtype=float)
        for j in range(len(x_nodes)):
            if j != i:
                numerator *= (x - x_nodes[j])
        Li = numerator / divider
        y += y_nodes[i] * Li

    return y
