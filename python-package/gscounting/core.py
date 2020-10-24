import ctypes
import os
import numpy as np
from libpath import find_lib_path


class GSCountingError(ValueError):
    """Error thrown by csr library."""


class SliceArgs(ctypes.Structure):
    _fields_ = [
        ('csr_handle', ctypes.c_void_p),
        ('idxset', ctypes.POINTER(ctypes.c_int)),
        ('len', ctypes.c_uint64),
        ('handle_out', ctypes.c_void_p),
        ('data_out', ctypes.POINTER(ctypes.c_float)),
        ('nrows_out', ctypes.c_uint64),
        ('ncols_out', ctypes.c_uint64),
    ]


def py_str(x):
    """convert c string back to python string"""
    return x.decode('utf-8')


def c_str(string):
    """Convert a python string to cstring."""
    return ctypes.c_char_p(string.encode('utf-8'))


def c_array(ctype, values):
    """Convert a python string to c array."""
    if (isinstance(values, np.ndarray)
            and values.dtype.itemsize == ctypes.sizeof(ctype)):
        return np.ctypeslib.as_ctypes(values)
    return (ctype * len(values))(*values)


def ctypes2numpy(cptr, shape):
    """Convert a ctypes pointer array to a numpy array."""
    ctype = ctypes.c_float
    if not isinstance(cptr, ctypes.POINTER(ctype)):
        raise RuntimeError('expected {} pointer'.format(ctype))

    return np.ctypeslib.as_array(cptr, shape)


def _load_lib():
    lib_path = find_lib_path()[0]
    lib = ctypes.cdll.LoadLibrary(lib_path)
    return lib


# load the GSCounting library globally
_LIB = _load_lib()
_LIB.DenseMatrixSliceCSRMatrix.argtypes = [ctypes.POINTER(SliceArgs)]


def _check_call(ret):
    """Check the return value of C API call
    This function will raise exception when error occurs.
    Wrap every API call with this function
    Parameters
    ----------
    ret : int
        return value from API calls
    """
    if ret != 0:
        raise GSCountingError("C API call error")
