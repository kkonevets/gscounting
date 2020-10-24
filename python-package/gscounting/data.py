import ctypes
import os

from core import _LIB, _check_call, c_str, c_array, ctypes2numpy, SliceArgs


class DenseMatrix:
    def __init__(self, handle, data, shape):
        self.handle = ctypes.c_void_p(handle)
        self.data = data
        self.shape = shape

    @property
    def numpy(self):
        if hasattr(self, "handle") and self.handle:
            return ctypes2numpy(self.data, self.shape)

    def __del__(self):
        if hasattr(self, "handle") and self.handle:
            _check_call(_LIB.DenseMatrixFree(self.handle))
            self.handle = None


class CSRMatrix:
    def __init__(self, fname):
        self.handle = ctypes.c_void_p()
        _check_call(
            _LIB.CSRMatrixLoadFromFile(c_str(os.fspath(fname)),
                                       ctypes.byref(self.handle)))

    def __getitem__(self, ixs):
        args = SliceArgs(
            self.handle,
            c_array(ctypes.c_int, ixs),
            ctypes.c_uint64(len(ixs)),
        )

        _check_call(_LIB.DenseMatrixSliceCSRMatrix(ctypes.byref(args)))

        return DenseMatrix(args.handle_out, args.data_out,
                           (args.nrows_out, args.ncols_out))

    def __del__(self):
        if hasattr(self, "handle") and self.handle:
            _check_call(_LIB.CSRMatrixFree(self.handle))
            self.handle = None