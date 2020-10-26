// "Copyright 2020 Kirill Konevets"

//!
//! @file csr_matrix.hpp
//! Compressed Sparse Row matrix parallel slicing implementation
//!

#ifndef INCLUDE_CSR_MATRIX_HPP_
#define INCLUDE_CSR_MATRIX_HPP_

#define TBB_SUPPRESS_DEPRECATED_MESSAGES 1

#include "tbb/tbb.h"
#include "tools.hpp"

#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

/** @struct CSR
 *
 *  Compressed Sparse Row matrix.
 *  Data format same as of scipy.sparse.csr_matrix.
 *  Is the standard CSR representation where the column indices
 *  for row `i` are stored in `indices[indptr[i]:indptr[i+1]]` and
 *  their corresponding values are stored in
 *  `data[indptr[i]:indptr[i+1]]`.
 */
struct CSR {
  using vec_f = std::vector<float>;
  using vec_u = std::vector<std::uint32_t>;

  vec_f _data;
  vec_u _indices;
  vec_u _indptr;
  size_t _nrows;
  size_t _ncols;

  vec_f _slice_data;

  explicit CSR(vec_f &&data, vec_u &&indices, vec_u &&indptr, size_t nrows,
               size_t ncols);

  /// first read vector size and then vector data
  template <class T>
  static auto _read_vector(std::istream &is) -> std::vector<T>;

  /// first forward write vector size and then vector data
  template <class T>
  static void _write_vector(std::ostream &os, const std::vector<T> &v);

  /// Load matrix from a binary format.
  /// First read matrix shape and then each vector with it's forward size.
  static auto load(const std::string &fname) -> CSR *;

  /// Saves matrix in a native endian (little endian mostly) binary format.
  /// First forward write matrix shape and then each vector with it's forward
  /// size.
  void save(const std::string &fname);

  /// Generate random csr matrix with probability of element being zero equal to
  /// `prob`
  static auto random(size_t nrows, size_t ncols, float prob) -> CSR;

  /**
   *  Performs parallel slicing on indexes.
   *  It splits `ixs` on chunks and each chunk is fed to a separate thread
   *  @param ixs List of ixs to slice on, must not be out of range
   */
  auto slice(const int *ixs, size_t size) -> std::vector<float> &;

  auto operator==(const CSR &o) const -> bool {
    return _ncols == o._ncols && _nrows == o._nrows && _data == o._data &&
           _indices == o._indices && _indptr == o._indptr;
  }
};

#endif // INCLUDE_CSR_MATRIX_HPP_
