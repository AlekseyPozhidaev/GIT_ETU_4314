import matplotlib.pyplot as plt
import numpy as np

from core.chebyshev import chebyshev_nodes, chebyshev
from core.error import get_max_error
from core.lagrange import lagrange
from core.newton import newton


def lagrange_plots(func, x_nodes, dif=100, ch_deg=5):
    x = np.linspace(min(x_nodes), max(x_nodes), dif)
    y = func(x)

    y_nodes = func(x_nodes)
    lagrangian_uniform = lagrange(x_nodes=x_nodes, y_nodes=y_nodes, x=x)
    uniform_error = (y - lagrangian_uniform)


    x_chebyshev = chebyshev_nodes(ch_deg, min(x_nodes), max(x_nodes))
    y_chebyshev = func(x_chebyshev)
    lagrangian_chebyshev = lagrange(x_nodes=x_chebyshev, y_nodes=y_chebyshev, x=x)
    chebyshev_error = (y - lagrangian_chebyshev)

    fig, axs = plt.subplots(2, 2, figsize=(8, 6))

    # Левый верхний
    axs[0, 0].plot(x, y, label="func")
    axs[0, 0].plot(x, lagrangian_uniform, label="Interpolation")
    axs[0, 0].set_title('Lagrange uniform')

    # Правый верхний
    axs[0, 1].plot(x, uniform_error, label="error")
    axs[0, 1].set_title('Lagrange uniform error')

    # Левый нижний
    axs[1, 0].plot(x, y, label="func")
    axs[1, 0].plot(x, lagrangian_chebyshev, label="Interpolation")
    axs[1, 0].set_title("Lagrange Chebyshev")

    # Правый нижний
    axs[1, 1].plot(x, chebyshev_error, label="error")
    axs[1, 1].set_title('Lagrange Chebyshev error')

    # Общие настройки
    for ax in axs.flat:
        ax.set_xlabel('x')
        ax.set_ylabel('y')
        ax.legend(fontsize=11)
        ax.grid(True)

    plt.tight_layout()
    plt.show()

def newton_plots(func, x_nodes, dif=100):
    x = np.linspace(min(x_nodes), max(x_nodes), dif)
    y = func(x)

    y_nodes = func(x_nodes)
    newton_uniform = newton(x_nodes=x_nodes, y_nodes=y_nodes, x=x)
    uniform_error = (y - newton_uniform)

    fig, axs = plt.subplots(2, figsize=(8, 6))

    # Левый верхний
    axs[0].plot(x, y, label="func")
    axs[0].plot(x, newton_uniform, label="Interpolation")
    axs[0].set_title('Newton uniform')

    # Правый верхний
    axs[1].plot(x, uniform_error, label="error")
    axs[1].set_title('Newton uniform error')


    # Общие настройки
    for ax in axs.flat:
        ax.set_xlabel('x')
        ax.set_ylabel('y')
        ax.legend(fontsize=11)
        ax.grid(True)

    plt.tight_layout()
    plt.show()

def chebyshev_plot(deg=5, dif=100):
    x = np.linspace(-1, 1, dif)
    y = chebyshev(deg, x)

    plt.plot(x, y)
    plt.title="Chebyshev"
    plt.xlabel("x")
    plt.ylabel("y")
    plt.legend(fontsize=11)
    plt.grid(True)

    plt.show()


def error_plot(func, n_min=3, n_max=15, dif=100):
    lag_un_errors = []
    lag_ch_errors = []
    new_un_errors = []
    n_range = np.linspace(n_min, n_max)

    for n in range(n_min, n_max + 1):
        x_nodes = np.linspace(-1, 1, n)
        ule, cle, une = get_max_error(func, x_nodes, dif)
        lag_un_errors.append(ule)
        lag_ch_errors.append(cle)
        new_un_errors.append(une)

    fig, axs = plt.subplots(2, figsize=(8, 6))

    # Левый верхний
    axs[0].plot(n_range, lag_un_errors, label="Lagrange unified error")
    axs[0].plot(n_range, lag_ch_errors, label="Lagrange Chebyshev error")
    axs[0].set_title('Lagrange error')

    # Правый верхний
    axs[1].plot(n_range, new_un_errors, label="error")
    axs[1].set_title('Newton error')

    # Общие настройки
    for ax in axs.flat:
        ax.set_xlabel('x')
        ax.set_ylabel('y')
        ax.legend(fontsize=11)
        ax.grid(True)

    plt.tight_layout()
    plt.show()