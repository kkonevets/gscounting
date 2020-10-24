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

typedef struct SliceArgs {
  CSRMatrixHandle csr_handle;
  const int *idxset;
  uint64_t len;
  DenseMatrixHandle handle_out;
  float *data_out;
  uint64_t nrows_out;
  uint64_t ncols_out;
} SliceArgs;

/*!
 * \brief load a CSR matrix
 * \param fname the name of the file
 * \param out a loaded CSR matrix
 * \return 0 when success, -1 when failure happens
 */
GSC_DLL int CSRMatrixLoadFromFile(const char *fname, CSRMatrixHandle *out);

/*!
 * \brief save a CSR matrix into binary file
 * \param handle an instance of CSR matrix
 * \param fname file name
 * \return 0 when success, -1 when failure happens
 */
GSC_DLL int CSRMatrixSaveBinary(CSRMatrixHandle handle, const char *fname);

/*!
 * \brief create a new dense matrix from sliced content of existing CSR matrix
 * \param handle instance of CSR matrix to be sliced
 * \param idxset index set
 * \param len length of index set
 * \param out a sliced new matrix
 * \return 0 when success, -1 when failure happens
 */
GSC_DLL int DenseMatrixSliceCSRMatrix(SliceArgs *args);

/*!
 * \brief free space in CSR matrix
 * \return 0 when success, -1 when failure happens
 */
GSC_DLL int CSRMatrixFree(CSRMatrixHandle handle);

/*!
 * \brief free space in dense matrix
 * \return 0 when success, -1 when failure happens
 */
GSC_DLL int DenseMatrixFree(DenseMatrixHandle handle);

#endif // GSCOUNTING_C_API_H_
