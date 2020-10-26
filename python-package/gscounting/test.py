import numpy as np
from data import CSRMatrix

test_a = np.array([[1, 0, 0], [0, 0, 0], [4, 5, 0]], 'f')

m = CSRMatrix("../../tests/data/m.bin")


def custom_print(a):
    l = []
    for row in a:
        for j in row:
            l.append(j)
    return l


def show():
    ixs = np.array([0, 1, 2], 'i')
    d = m[ixs]
    print((d.numpy == test_a).all())
    print(d.numpy)
    print((d.numpy == test_a).all())
    print(d.numpy)
    print()


show()
show()
show()
show()
