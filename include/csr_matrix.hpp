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
#include <chrono>
#include <cstddef>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
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
  Dense(size_t nrows, size_t ncols, std::vector<float> &&init_data)
      : nrows{nrows}, ncols{ncols}, data{std::move(init_data)} {
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

  vec_f _data;
  vec_u _indices;
  vec_u _indptr;
  size_t _nrows;
  size_t _ncols;

  explicit CSR(vec_f &&data, vec_u &&indices, vec_u &&indptr, size_t nrows = 0,
               size_t ncols = 0)
      : _data(std::move(data)), _indices(std::move(indices)),
        _indptr(std::move(indptr)), _nrows(nrows), _ncols(ncols) {
    if (_indices.empty()) {
      throw std::runtime_error("indices array is empty");
    }
    if (_indptr.empty()) {
      throw std::runtime_error("index pointer array is empty");
    }
    if (_indptr[0] != 0) {
      throw std::runtime_error("index pointer array should start with 0");
    }
    if (_data.size() != _indices.size()) {
      throw std::runtime_error("indices and data arrays should have same size");
    }
    if (_indptr.back() > _indices.size()) {
      throw std::runtime_error(
          "Last value of index pointer should be less than "
          "the size of index and data arrays");
    }
    if (!std::is_sorted(_indptr.begin(), _indptr.end())) {
      throw std::runtime_error("index pointer values must form a "
                               "non-decreasing sequence");
    }

    auto infered_nrows = _indptr.size() - 1;
    auto it = std::max_element(_indices.begin(), _indices.end());
    auto infered_ncols = static_cast<size_t>(*it) + 1;
    if ((_ncols ^ _nrows) != 0) {
      throw std::runtime_error(
          "both nrows and ncols should be provided or none");
    }
    if (_ncols != 0 && _nrows != 0) {
      if (infered_nrows > _nrows || infered_ncols > _ncols) {
        throw std::runtime_error("shape is too small");
      }
    }
    if (_ncols == 0 && _nrows == 0) {
      _ncols = infered_ncols;
      _nrows = infered_nrows;
    }
  }

  /// Load matrix from a binary format.
  /// First it reads matrix shape and then each vector with it's
  /// forward size.
  static auto load(const std::string &fname) {
    std::ifstream is(fname);
    if (!is) {
      throw std::runtime_error(fname);
    }

    std::uint32_t nrows;
    std::uint32_t ncols;
    is.read(reinterpret_cast<char *>(&nrows), sizeof(std::uint32_t));
    is.read(reinterpret_cast<char *>(&ncols), sizeof(std::uint32_t));

    auto data = _read_vector<float>(is);
    auto indices = _read_vector<std::uint32_t>(is);
    auto indptr = _read_vector<std::uint32_t>(is);

    auto m = CSR(std::move(data), std::move(indices), std::move(indptr));
    return m;
  }

  /// first read vector size and then vector data
  template <class T>
  static auto _read_vector(std::istream &is) -> std::vector<T> {
    std::uint32_t _v_size;
    is.read(reinterpret_cast<char *>(&_v_size), sizeof(std::uint32_t));

    std::vector<T> v;
    v.resize(_v_size);
    is.read(reinterpret_cast<char *>(v.data()), _v_size * sizeof(T));
    return v;
  }

  /// first forward write vector size and then vector data
  template <class T>
  static void _write_vector(std::ostream &os, const std::vector<T> &v) {
    auto _v_size(static_cast<std::uint32_t>(v.size()));
    os.write(reinterpret_cast<const char *>(&(_v_size)), sizeof(std::uint32_t));
    os.write(reinterpret_cast<const char *>(v.data()), v.size() * sizeof(T));
  }

  /// Saves matrix in a native endian (little endian mostly) binary format.
  /// First it forward writes matrix shape and then each vector with it's
  /// forward size.
  void save(const std::string &fname) {
    std::ofstream os(fname, std::ios::binary);
    if (!os) {
      throw std::runtime_error(fname);
    }

    // forward write matrix shape
    os.write(reinterpret_cast<const char *>(&_nrows), sizeof(std::uint32_t));
    os.write(reinterpret_cast<const char *>(&_ncols), sizeof(std::uint32_t));

    _write_vector(os, _data);
    _write_vector(os, _indices);
    _write_vector(os, _indptr);
  }

  /// Generate random csr matrix with probability of element being zero equal to
  /// `prob`
  static auto random(size_t nrows, size_t ncols, float prob) -> CSR {
    std::vector<float> data;
    std::vector<std::uint32_t> indices;
    std::vector<std::uint32_t> indptr{0};

    std::mt19937 rng(
        std::chrono::steady_clock::now().time_since_epoch().count());

    auto threshold = static_cast<unsigned int>(prob * 100);
    size_t iptr = 0;
    for (size_t _i = 0; _i < nrows; ++_i) {
      for (size_t j = 0; j < ncols; ++j) {
        auto r = rng();
        if (r % 100 < threshold) {
          float v = static_cast<float>(r) / static_cast<float>(rng.max());
          data.push_back(v);
          indices.push_back(j);
          iptr++;
        }
      }
      indptr.push_back(iptr);
    }
    auto m = CSR(std::move(data), std::move(indices), std::move(indptr));
    return m;
  }
  auto slice(const std::vector<std::uint32_t> &ixs) -> Dense;
};

/**
 *  Performs parallel slicing on indexes.
 *  It splits `ixs` on chunks and each chunk is fed to a separate thread
 *  @param ixs List of ixs to slice on, must not be out of range
 */
auto CSR::slice(const std::vector<std::uint32_t> &ixs) -> Dense {
  Dense d(ixs.size(), _ncols);

  auto worker = [&](const tbb::blocked_range<size_t> &r) {
    for (auto i = r.begin(); i != r.end(); ++i) {
      size_t ix = ixs[i];
      if (ix > _nrows) {
        fprintf(stderr, "Index %zu is out of range (0, %zu)\n", ix, _nrows);
        return;
      }
      auto _i = i * _ncols;
      for (size_t j = _indptr[ix]; j < _indptr[ix + 1]; ++j) {
        d.data[_i + _indices[j]] = _data[j];
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
      os << std::setw(11) << std::left << std::setprecision(5)
         << d.data[_i + j];
    }
    os << std::endl;
  }
  return os;
}

} // namespace std

#endif // INCLUDE_CSR_MATRIX_HPP_
