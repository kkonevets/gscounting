// "Copyright 2020 Kirill Konevets"

//!
//! @file csr_matrix.hpp
//! Compressed Sparse Row matrix parallel slicing implementation
//!

#ifndef INCLUDE_CSR_MATRIX_HPP_
#define INCLUDE_CSR_MATRIX_HPP_

#include "tbb/tbb.h"
#include "tools.hpp"

#include <cassert>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

/** @struct Dense
 *
 *  Dense matrix representation.
 */
struct Dense {
  size_t nrows;
  size_t ncols;
  /// matrix data as contiguous array of concatenated rows
  std::vector<float> data;

  Dense(size_t nrows, size_t ncols)
      : nrows{nrows}, ncols{ncols}, data(nrows * ncols, 0) {}
  Dense(size_t nrows, size_t ncols, std::vector<float> &&init_data)
      : nrows{nrows}, ncols{ncols}, data{std::move(init_data)} {
    if (nrows * ncols != data.size()) {
      throw std::runtime_error("indices array is empty");
    }
  }
  Dense(Dense &&other) noexcept
      : nrows{other.nrows}, ncols{other.ncols}, data{std::move(other.data)} {}
};

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

  explicit CSR(vec_f &&data, vec_u &&indices, vec_u &&indptr, size_t nrows,
               size_t ncols);

  template <class T>
  static auto _read_vector(std::istream &is) -> std::vector<T>;

  template <class T>
  static void _write_vector(std::ostream &os, const std::vector<T> &v);

  static auto load(const std::string &fname) -> CSR;

  void save(const std::string &fname);

  static auto random(size_t nrows, size_t ncols, float prob) -> CSR;

  auto slice(const std::vector<std::uint32_t> &ixs) -> Dense;

  auto operator==(const CSR &o) -> bool {
    bool equal = _ncols == o._ncols && _nrows == o._nrows && _data == o._data &&
                 _indices == o._indices && _indptr == o._indptr;
    return equal;
  }
};

#endif // INCLUDE_CSR_MATRIX_HPP_
