import numpy as np
from data import CSRMatrix

m = CSRMatrix("../../tests/data/m.bin")
ixs = np.array([0, 1, 2], dtype=np.int)
d = m[ixs]
d.numpy
