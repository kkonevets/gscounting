# coding: utf-8
"""Find the path to gscounting dynamic library files."""

import os
import platform
import sys


class GSCountingtLibraryNotFound(Exception):
    """Error thrown by when gsc is not found"""


def find_lib_path():
    """Find the path to gsc dynamic library files.

    Returns
    -------
    lib_path: list(string)
       List of all found library path to gsc
    """
    curr_path = os.path.dirname(os.path.abspath(os.path.expanduser(__file__)))
    dll_path = [
        # normal, after installation `lib` is copied into Python package tree.
        os.path.join(curr_path, 'lib'),
        # editable installation, no copying is performed.
        os.path.join(curr_path, os.path.pardir, os.path.pardir, 'build',
                     'lib'),
    ]
    if sys.platform == 'win32':
        if platform.architecture()[0] == '64bit':
            dll_path.append(
                os.path.join(curr_path, '../../windows/x64/Release/'))
            # hack for pip installation when copy all parent source
            # directory here
            dll_path.append(os.path.join(curr_path, './windows/x64/Release/'))
        else:
            dll_path.append(os.path.join(curr_path, '../../windows/Release/'))
            # hack for pip installation when copy all parent source
            # directory here
            dll_path.append(os.path.join(curr_path, './windows/Release/'))
        dll_path = [os.path.join(p, 'gsc.dll') for p in dll_path]
    elif sys.platform.startswith('linux') or sys.platform.startswith(
            'freebsd'):
        dll_path = [os.path.join(p, 'libgsc.so') for p in dll_path]
    elif sys.platform == 'darwin':
        dll_path = [os.path.join(p, 'libgsc.dylib') for p in dll_path]
    elif sys.platform == 'cygwin':
        dll_path = [os.path.join(p, 'cyggsc.dll') for p in dll_path]

    lib_path = [p for p in dll_path if os.path.exists(p) and os.path.isfile(p)]

    if not lib_path:
        msg = 'Cannot find GSCounting Library in the candidate path.  ' + \
            'List of candidates:\n- ' + ('\n- '.join(dll_path)) + \
            '\GSCounting Python package path: ' + curr_path + \
            '\nsys.prefix: ' + sys.prefix
        raise GSCountingtLibraryNotFound(msg)
    return lib_path
