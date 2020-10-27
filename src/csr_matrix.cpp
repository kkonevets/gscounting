#include "csr_matrix.hpp"

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

CSR::CSR(vec_f &&data, vec_u &&indices, vec_u &&indptr, size_t nrows = 0,
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
    throw std::runtime_error("Last value of index pointer should be less than "
                             "the size of index and data arrays");
  }
  if (!std::is_sorted(_indptr.begin(), _indptr.end())) {
    throw std::runtime_error("index pointer values must form a "
                             "non-decreasing sequence");
  }

  auto infered_nrows = _indptr.size() - 1;
  auto it = std::max_element(_indices.begin(), _indices.end());
  auto infered_ncols = static_cast<size_t>(*it) + 1;
  if ((!_ncols != !_nrows) != 0) { // logical XOR
    throw std::runtime_error("both nrows and ncols should be provided or none");
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

template <class T> auto CSR::_read_vector(std::istream &is) -> std::vector<T> {
  std::uint32_t _v_size(0);
  is.read(reinterpret_cast<char *>(&_v_size), sizeof(std::uint32_t));

  std::vector<T> v;
  v.resize(_v_size);
  is.read(reinterpret_cast<char *>(v.data()), _v_size * sizeof(T));
  return v;
}

template <class T>
void CSR::_write_vector(std::ostream &os, const std::vector<T> &v) {
  auto _v_size(static_cast<std::uint32_t>(v.size()));
  os.write(reinterpret_cast<const char *>(&(_v_size)), sizeof(std::uint32_t));
  os.write(reinterpret_cast<const char *>(v.data()), v.size() * sizeof(T));
}

auto CSR::load(const std::string &fname) -> CSR * {
  std::ifstream is(fname);
  if (!is) {
    throw std::runtime_error(fname);
  }

  std::uint32_t nrows(0);
  std::uint32_t ncols(0);
  is.read(reinterpret_cast<char *>(&nrows), sizeof(std::uint32_t));
  is.read(reinterpret_cast<char *>(&ncols), sizeof(std::uint32_t));

  auto data = CSR::_read_vector<float>(is);
  auto indices = CSR::_read_vector<std::uint32_t>(is);
  auto indptr = CSR::_read_vector<std::uint32_t>(is);

  return new CSR(std::move(data), std::move(indices), std::move(indptr), nrows,
                 ncols);
}

void CSR::save(const std::string &fname) {
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

auto CSR::random(size_t nrows, size_t ncols, float prob) -> CSR {
  std::vector<float> data;
  std::vector<std::uint32_t> indices;
  std::vector<std::uint32_t> indptr{0};

  auto seed(std::chrono::steady_clock::now().time_since_epoch().count());
  std::mt19937 rng(seed);

  auto threshold = static_cast<unsigned int>(prob * 100);
  size_t iptr = 0;
  for (size_t _i = 0; _i < nrows; ++_i) {
    for (size_t j = 0; j < ncols; ++j) {
      auto r = rng();
      if (r % 100 < threshold) {
        auto v = static_cast<float>(r) / static_cast<float>(rng.max());
        data.push_back(v);
        indices.push_back(j);
        iptr++;
      }
    }
    indptr.push_back(iptr);
  }
  return CSR(std::move(data), std::move(indices), std::move(indptr));
}

auto CSR::slice(const int *ixs, size_t size) -> float * {
  auto prev_size = _slice_data.size();
  auto new_size = size * _ncols;
  _slice_data.resize(new_size, 0);

  auto begin = _slice_data.begin();
  auto end = begin + std::min(prev_size, new_size);
  std::fill(begin, end, 0);

  for (size_t i = 0; i < size; ++i) {
    auto ixi{ixs[i]};
    if (ixi < 0) {
      ixi += _nrows;
    }
    size_t ix = static_cast<size_t>(ixi);
    if (ix >= _nrows) {
      std::ostringstream ss;
      ss << "Index " << ix << " is out of range (0, " << _nrows << ")";
      throw std::runtime_error(ss.str());
    }
    auto _i = i * _ncols;
    for (size_t j = _indptr[ix]; j < _indptr[ix + 1]; ++j) {
      _slice_data[_i + _indices[j]] = _data[j];
    }
  }

  return _slice_data.data();
}
