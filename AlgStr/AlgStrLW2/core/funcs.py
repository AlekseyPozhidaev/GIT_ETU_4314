import numpy as np

def linear(x, a=2.0, b=1.0):
    """y = a*x + b"""
    x = np.array(x)
    return a * x + b

def quadratic(x, a=1.0, b=0.0, c=0.0):
    """y = a*x^2 + b*x + c"""
    x = np.array(x)
    return a * x**2 + b * x + c

def cubic(x, a=1.0, b=0.0, c=0.0, d=0.0):
    """y = a*x^3 + b*x^2 + c*x + d"""
    x = np.array(x)
    return a * x**3 + b * x**2 + c * x + d

def sine_wave(x, A=1.0, freq=1.0, phase=0.0):
    """y = A * sin(2*pi*freq*x + phase)"""
    x = np.array(x)
    return A * np.sin(2 * np.pi * freq * x + phase)

def damped_cosine(x, A=1.0, freq=1.0, decay=0.5, phase=0.0):
    """y = A * exp(-decay*x) * cos(2*pi*freq*x + phase)"""
    x = np.array(x)
    return A * np.exp(-decay * x) * np.cos(2 * np.pi * freq * x + phase)

def exponential(x, base=np.e, scale=1.0):
    """y = base^(scale * x)  (default is e^(x))"""
    x = np.array(x)
    return base ** (scale * x)

def logarithm(x, base=np.e, shift=1.5):
    """
    Безопасный логарифм для интервала [-1, 1]
    y = log_base(x + shift)
    shift=1.5 → аргумент от 0.5 до 2.5 → всё хорошо определено
    """
    x = np.asarray(x, dtype=float)
    return np.log(x + shift) / np.log(base)

def gaussian(x, mu=0.0, sigma=1.0):
    """y = exp(-0.5*((x-mu)/sigma)^2) / (sigma*sqrt(2*pi))"""
    x = np.array(x)
    coef = 1.0 / (sigma * np.sqrt(2 * np.pi))
    return coef * np.exp(-0.5 * ((x - mu) / sigma) ** 2)

def sigmoid(x, k=1.0, x0=0.0):
    """y = 1 / (1 + exp(-k*(x - x0)))"""
    x = np.array(x)
    return 1.0 / (1.0 + np.exp(-k * (x - x0)))
