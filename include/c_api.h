/*!
 * Copyright (c) 2020 by Contributors
 * \file c_api.h
 * \author Kirill Konevets
 * \brief C API of GSCounting, used for interfacing to other languages.
 */
#ifndef GSCOUNTING_C_API_H_
#define GSCOUNTING_C_API_H_

#ifdef __cplusplus
#define GSC_EXTERN_C extern "C"
#include <cstdint>
#include <cstdio>
#else
#define GSC_EXTERN_C
#include <stdint.h>
#include <stdio.h>
#endif // __cplusplus

#if defined(_MSC_VER) || defined(_WIN32)
#define GSC_DLL GSC_EXTERN_C __declspec(dllexport)
#else
#define GSC_DLL GSC_EXTERN_C __attribute__((visibility("default")))
#endif // defined(_MSC_VER) || defined(_WIN32)

typedef void *CSRMatrixHandle;
typedef void *DenseMatrixHandle;

/// Represents set of arguments to call `DenseMatrixSliceCSRMatrix()`
typedef struct SliceArgs {
  /// handle to CSR matrix
  CSRMatrixHandle handle;
  /// indices to slice with
  const int *idxset;
  /// length of `idxset`
  uint64_t len;
  /// poiner to a contiguous data array of Dense matrix
  float *data_out;
} SliceArgs;

typedef struct LoadArgs {
  /// name of file to load
  const char *fname;
  /// handle to CSR matrix
  CSRMatrixHandle handle;
  /// number of rows in Dense matrix
  uint64_t nrows_out;
  /// number of columns in Dense matrix
  uint64_t ncols_out;
} LoadArgs;

/*!
 * \brief load a CSR matrix
 * \param args pointer to LoadArgs
 * \return 0 when success, -1 when failure happens
 */
GSC_DLL int CSRMatrixLoadFromFile(LoadArgs *args);

/*!
 * \brief save a CSR matrix into binary file
 * \param handle an instance of CSR matrix
 * \param fname file name
 * \return 0 when success, -1 when failure happens
 */
GSC_DLL int CSRMatrixSaveBinary(CSRMatrixHandle handle, const char *fname);

/*!
 * \brief create a new Dense matrix as a slice of existing CSR matrix
 * \param args pointer to SliceArgs, output is written back to `args`
 * \return 0 when success, -1 when failure happens
 */
GSC_DLL int DenseMatrixSliceCSRMatrix(SliceArgs *args);

/*!
 * \brief free space in CSR matrix
 * \return 0 when success, -1 when failure happens
 */
GSC_DLL int CSRMatrixFree(CSRMatrixHandle handle);

#endif // GSCOUNTING_C_API_H_
