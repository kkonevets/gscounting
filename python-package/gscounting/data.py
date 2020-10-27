import ctypes
import os

from core import _LIB, _check_call, c_str, c_array, ctypes2numpy, SliceArgs, LoadArgs


class DenseMatrix:
    def __init__(self, data, shape):
        self.data = data
        self._shape = shape

    @property
    def numpy(self):
        if hasattr(self, "data") and self.data:
            return ctypes2numpy(self.data, self.shape)

    @property
    def shape(self):
        return self._shape

    def __del__(self):
        if hasattr(self, "handle") and self.handle:
            _check_call(_LIB.DenseMatrixFree(self.handle))
            self.handle = None


class CSRMatrix:
    def __init__(self, fname):
        args = LoadArgs(c_str(os.fspath(fname)), )
        _check_call(_LIB.CSRMatrixLoadFromFile(ctypes.byref(args)))
        self.handle = args.handle
        self._shape = (args.nrows_out, args.ncols_out)

    @property
    def shape(self):
        return self._shape

    def __getitem__(self, ixs):
        args = SliceArgs(
            self.handle,
            c_array(ctypes.c_int, ixs),
            ctypes.c_uint64(len(ixs)),
        )

        _check_call(_LIB.DenseMatrixSliceCSRMatrix(ctypes.byref(args)))

        return DenseMatrix(args.data_out, (len(ixs), self.shape[1]))

    def __del__(self):
        if hasattr(self, "handle") and self.handle:
            _check_call(_LIB.CSRMatrixFree(self.handle))
            self.handle = None
