import numpy as np

import numpy as np

def chebyshev(deg, x):
    if deg < 0:
        raise ValueError("deg must be non-negative")
    x_arr = np.asarray(x, dtype=float)


    # базовые случаи
    if deg == 0:
        result = np.ones_like(x_arr, dtype=float)
        return float(result) if np.isscalar(x) else result
    if deg == 1:
        result = x_arr.copy()
        return float(result) if np.isscalar(x) else result

    t0 = np.array(x_arr, dtype=float)
    t1 = x_arr.copy()

    for _ in range(2, deg + 1):
        t2 = 2 * x_arr * t1 - t0
        t0, t1 = t1, t2

    return t1


def chebyshev_nodes(deg, a=-1, b=1):
    k = np.arange(deg)
    x = np.cos((2*k+1)/(2*deg) * np.pi)
    return 0.5*(a+b) + 0.5*(b-a)*x
