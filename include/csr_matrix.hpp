// "Copyright 2020 Kirill Konevets"

//!
//! @file csr_matrix.hpp
//! Compressed Sparse Row matrix parallel slicing implementation
//!

#ifndef INCLUDE_CSR_MATRIX_HPP_
#define INCLUDE_CSR_MATRIX_HPP_

#include "tbb/tbb.h"
#include "tools.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <utility>
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
  Dense(size_t nrows, size_t ncols, std::vector<float> &&data)
      : nrows{nrows}, ncols{ncols}, data{std::move(data)} {
    assert(nrows * ncols == data.size());
  }
  Dense(Dense &&other) noexcept
      : nrows{other.nrows}, ncols{other.ncols}, data{std::move(other.data)} {
    assert(nrows * ncols == data.size());
  }
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

  vec_f data;
  vec_u indices;
  vec_u indptr;
  size_t nrows;
  size_t ncols;

  explicit CSR(vec_f &&data, vec_u &&indices, vec_u &&indptr)
      : data(std::move(data)),       //
        indices(std::move(indices)), //
        indptr(std::move(indptr)) {
    assert(check_csr());
    nrows = this->indptr.size() - 1;
    auto it = std::max_element(this->indices.begin(), this->indices.end());
    ncols = static_cast<size_t>(*it) + 1;
  }
  ///  Loads 3 binary files and constructs CSR matrix
  static auto load(const std::string &data_path,
                   const std::string &indices_path,
                   const std::string &indptr_path) {
    auto data = read_vec<float>(data_path);
    auto indices = read_vec<std::uint32_t>(indices_path);
    auto indptr = read_vec<std::uint32_t>(indptr_path);
    auto m = CSR(std::move(data), std::move(indices), std::move(indptr));
    return m;
  }
  /// Generate random csr matrix with probability of element being zero equal to
  /// `prob`
  static auto random(size_t nrows, size_t ncols, float prob) -> CSR {
    std::vector<float> data;
    std::vector<std::uint32_t> indices;
    std::vector<std::uint32_t> indptr{0};

    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution dist(prob);

    size_t iptr = 0;
    for (size_t i = 0; i < nrows; ++i) {
      for (size_t j = 0; j < ncols; ++j) {
        if (dist(gen)) {
          data.push_back(1);
          indices.push_back(j);
          iptr++;
        }
      }
      indptr.push_back(iptr);
    }
    auto m = CSR(std::move(data), std::move(indices), std::move(indptr));
    return m;
  }
  auto check_csr() -> bool;
  auto slice(const std::vector<std::uint32_t> &ixs) -> Dense;
};

/**
 *  Validates data format
 *  @return true if valid, false otherwise
 */
auto CSR::check_csr() -> bool {
  if (indices.empty()) {
    std::cerr << "indices array is empty" << std::endl;
    return false;
  }
  if (indptr.empty()) {
    std::cerr << "index pointer array is empty" << std::endl;
    return false;
  }
  if (indptr[0] != 0) {
    std::cerr << "index pointer array should start with 0" << std::endl;
    return false;
  }
  if (data.size() != indices.size()) {
    std::cerr << "indices and data arrays should have same size" << std::endl;
    return false;
  }
  if (indptr.back() > indices.size()) {
    std::cerr << "Last value of index pointer should be less than "
                 "the size of index and data arrays"
              << std::endl;
    return false;
  }
  if (!std::is_sorted(indptr.begin(), indptr.end())) {
    std::cerr << "index pointer values must form a "
                 "non-decreasing sequence"
              << std::endl;
    return false;
  }
  return true;
}

/**
 *  Performs parallel slicing on indexes.
 *  It splits `ixs` on chunks and each chunk is fed to a separate thread
 *  @param ixs List of ixs to slice on, must not be out of range
 */
auto CSR::slice(const std::vector<std::uint32_t> &ixs) -> Dense {
  Dense d(ixs.size(), ncols);

  auto worker = [&](const tbb::blocked_range<size_t> &r) {
    for (auto i = r.begin(); i != r.end(); ++i) {
      size_t ix = ixs[i];
      if (ix > nrows) {
        fprintf(stderr, "Index %zu is out of range (0, %zu)\n", ix, nrows);
        return;
      }
      auto _i = i * ncols;
      for (size_t j = indptr[ix]; j < indptr[ix + 1]; ++j) {
        d.data[_i + indices[j]] = data[j];
      }
    }
  };

  parallel_for(tbb::blocked_range<size_t>(0, ixs.size()), worker);
  return d;
}

// ----------------------------------------------------------------------------
// Namespace std
// ----------------------------------------------------------------------------

namespace std {

auto operator<<(std::ostream &os, Dense &d) -> std::ostream & {
  for (size_t i = 0; i < d.nrows; ++i) {
    auto _i = i * d.ncols;
    for (size_t j = 0; j < d.ncols; ++j) {
      os << std::setw(10) << std::left << std::setprecision(10)
         << d.data[_i + j];
    }
    os << std::endl;
  }
  return os;
}

} // namespace std

#endif // INCLUDE_CSR_MATRIX_HPP_
