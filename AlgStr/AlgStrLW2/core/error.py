import numpy as np

from core.chebyshev import chebyshev_nodes
from core.lagrange import lagrange
from core.newton import newton


def get_max_error(func, x_nodes, dif=500):
    """
    Возвращает максимальную абсолютную ошибку для трёх методов:
    - Лагранж на равностоящих узлах
    - Лагранж на узлах Чебышева
    - Ньютон на равностоящих узлах
    """
    a, b = min(x_nodes), max(x_nodes)
    x = np.linspace(a, b, dif)
    y = func(x)

    # Лагранж равномерный
    y_nodes = func(x_nodes)
    lag_uniform = lagrange(x_nodes, y_nodes, x)
    err_lag_uniform = np.max(np.abs(y - lag_uniform))

    # Лагранж Чебышев
    x_cheb = chebyshev_nodes(len(x_nodes), a, b)
    y_cheb = func(x_cheb)
    lag_cheb = lagrange(x_cheb, y_cheb, x)
    err_lag_cheb = np.max(np.abs(y - lag_cheb))

    # Ньютон
    newt = newton(x_nodes, y_nodes, x)
    err_newton = np.max(np.abs(y - newt))

    return err_lag_uniform, err_lag_cheb, err_newton