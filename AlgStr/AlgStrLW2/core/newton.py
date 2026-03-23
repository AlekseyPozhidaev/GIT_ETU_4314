import numpy as np

def divided_differences(x_nodes, y_nodes):
    x = np.asarray(x_nodes, dtype=float)
    coef = np.asarray(y_nodes, dtype=float).copy()
    n = len(x)
    for j in range(1, n):
        for i in range(n-1, j-1, -1):
            coef[i] = (coef[i] - coef[i-1]) / (x[i] - x[i-j])
    return coef

def newton(x_nodes, y_nodes, x):
    x_nodes = np.asarray(x_nodes, dtype=float)
    coef = divided_differences(x_nodes, y_nodes)
    x_arr = np.asarray(x)
    result = np.full_like(x_arr, coef[0], dtype=float)
    product = np.ones_like(x_arr, dtype=float)
    for i in range(1, len(coef)):
        product *= x_arr - x_nodes[i-1]
        result += coef[i] * product
    return result

