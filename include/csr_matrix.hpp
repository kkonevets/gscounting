// "Copyright 2020 Kirill Konevets"

//!
//! @file csr_matrix.hpp
//! @brief Data format same as of scipy.sparse.csr_matrix.
//! Is the standard CSR representation where the column indices
//! for row i are stored in indices[indptr[i]:indptr[i+1]] and
//! their corresponding values are  stored in
//! data[indptr[i]:indptr[i+1]]. If the shape parameter is not supplied,
//! the matrix dimensions are inferred from the index arrays.
//!

#ifndef INCLUDE_CSR_MATRIX_HPP_
#define INCLUDE_CSR_MATRIX_HPP_

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "tbb/tbb.h"
#include "tools.hpp"

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
    nrows = this->indptr.size() - 1;
    assert(!this->indices.empty());
    auto it = std::max_element(this->indices.begin(), this->indices.end());
    ncols = static_cast<size_t>(*it) + 1;
  }
};

struct Dense {
  size_t nrows;
  size_t ncols;
  std::vector<float> data;

  Dense(size_t nrows, size_t ncols)
      : nrows{nrows}, ncols{ncols}, data(nrows * ncols, 0) {}
  Dense(size_t nrows, size_t ncols, std::vector<float> &&data)
      : nrows{nrows}, ncols{ncols}, data{std::move(data)} {
    assert(nrows * ncols == data.size());
  }
  Dense(const Dense &&other)
      : nrows{other.nrows}, ncols{other.ncols}, data{std::move(other.data)} {
    assert(nrows * ncols == data.size());
  }
};

bool check_csr(const CSR &m) {
  if (m.indptr.empty()) {
    std::cerr << "index pointer array is empty" << std::endl;
    return false;
  }
  if (m.indptr[0] != 0) {
    std::cerr << "index pointer array should start with 0" << std::endl;
    return false;
  }
  if (m.data.size() != m.indices.size()) {
    std::cerr << "indices and data arrays should have same size" << std::endl;
    return false;
  }
  if (m.indptr.back() > m.indices.size()) {
    std::cerr << "Last value of index pointer should be less than "
                 "the size of index and data arrays"
              << std::endl;
    return false;
  }
  if (!std::is_sorted(m.indptr.begin(), m.indptr.end())) {
    std::cerr << "index pointer values must form a "
                 "non-decreasing sequence"
              << std::endl;
    return false;
  }
  return true;
}

std::optional<CSR> init_csr(const std::string &data_path,
                            const std::string &indices_path,
                            const std::string &indptr_path) {
  auto data = read_vec<float>(data_path);
  auto indices = read_vec<std::uint32_t>(indices_path);
  auto indptr = read_vec<std::uint32_t>(indptr_path);
  auto m = CSR(std::move(data), std::move(indices), std::move(indptr));

  if (!check_csr(m)) {
    return {};
  }

  return m;
}

Dense slice(const CSR &m, const std::vector<std::uint32_t> &ixs) {
  assert(m.nrows != 0 && m.ncols != 0);
  Dense d(ixs.size(), m.ncols);

  auto worker = [&](const tbb::blocked_range<size_t> &r) {
    for (auto i = r.begin(); i != r.end(); ++i) {
      size_t ix = ixs[i];
      if (ix > m.nrows) {
        fprintf(stderr, "Index %zu is out of range (0, %zu)\n", ix, m.nrows);
        return;
      }
      auto _i = i * m.ncols;
      for (size_t j = m.indptr[ix]; j < m.indptr[ix + 1]; ++j) {
        d.data[_i + m.indices[j]] = m.data[j];
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

std::ostream &operator<<(std::ostream &os, Dense &d) {
  for (size_t i = 0; i < d.nrows; ++i) {
    auto _i = i * d.ncols;
    for (size_t j = 0; j < d.ncols; ++j) {
      os << d.data[_i + j] << " ";
    }
    os << std::endl;
  }
  return os;
}

} // namespace std

#endif // INCLUDE_CSR_MATRIX_HPP_
