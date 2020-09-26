// "Copyright 2020 Kirill Konevets"

#ifndef INCLUDE_TOOLS_HPP_
#define INCLUDE_TOOLS_HPP_

#include <fstream>
#include <iostream>
#include <istream>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// ----------------------------------------------------------------------------
// Edge and adjacency items definition
// ----------------------------------------------------------------------------

/** @struct EdgeItem

@brief structure representiong a directed edge "source->target" in a graph
*/
template <class T> struct EdgeItem {
  T source{0}, target{0};

  explicit EdgeItem(T source, T target) : source(source), target(target) {}
  EdgeItem() {}

  bool encode(std::ostream &os) {
    os.write(reinterpret_cast<char *>(&source), sizeof(T));
    os.write(reinterpret_cast<char *>(&target), sizeof(T));
    return os.good();
  }

  static bool decode(std::istream &is, EdgeItem<T> &edge) {
    is.read(reinterpret_cast<char *>(&edge.source), sizeof(T));
    is.read(reinterpret_cast<char *>(&edge.target), sizeof(T));
    return is.good();
  }
};

/** @struct AdjItem

@brief structure representiong a record in adjacency list

k is a source node and v contains a list of target nodes.
*/
template <class T> struct AdjItem {
  T k{0};
  std::vector<T> v;

  explicit AdjItem(T k, std::vector<T> &&v) : k(k), v(v) {}
  AdjItem() {}

  bool encode(std::ostream &os) {
    T len{static_cast<T>(v.size())};

    os.write(reinterpret_cast<char *>(&len), sizeof(T));
    os.write(reinterpret_cast<char *>(&k), sizeof(T));
    os.write(reinterpret_cast<char *>(v.data()), v.size() * sizeof(T));
    return os.good();
  }

  static bool decode(std::istream &is, AdjItem<T> &row) {
    T len;
    is.read(reinterpret_cast<char *>(&len), sizeof(T));
    is.read(reinterpret_cast<char *>(&row.k), sizeof(T));

    row.v.resize(len);
    is.read(reinterpret_cast<char *>(row.v.data()), len * sizeof(T));
    return is.good();
  }
};

// ----------------------------------------------------------------------------
// Iteration on edge and adjacency lists
// ----------------------------------------------------------------------------

class list_iterator_sentinel {};

template <class T> class list_iterator {
  std::istream &is;
  T curr;
  bool finish;

public:
  explicit list_iterator(std::istream &is) : is{is} {
    finish = T::decode(is, curr);
  }

  T &operator*() { return curr; }

  list_iterator &operator++() {
    finish = T::decode(is, curr);
    return *this;
  }

  bool operator!=(const list_iterator_sentinel) const { return finish; }
};

/** @class list_range

@brief iteration on edge and adjacency lists

T could be EdgeItem or AdjItem.
*/
template <class T> class list_range {
  std::istream &is;

public:
  explicit list_range(std::istream &is) : is{is} {}

  list_iterator<T> begin() const { return list_iterator<T>{is}; }
  list_iterator_sentinel end() const { return {}; }
};

// ----------------------------------------------------------------------------
// Miscellaneous
// ----------------------------------------------------------------------------

namespace std {

template <class T>
std::istream &operator>>(std::istream &is, EdgeItem<T> &edge) {
  is >> edge.source >> edge.target;
  return is;
}
} // namespace std

template <class T> std::vector<T> read_vec(const std::string &fname) {
  std::ifstream fin(fname, std::ios::binary | std::ios::ate);
  fin.exceptions(std::ifstream::badbit);
  std::streamsize size = fin.tellg();
  if (size % sizeof(T) != 0) {
    throw "file size is not multiple of type size";
  }
  fin.seekg(0, std::ios::beg);

  std::vector<T> buffer(size / sizeof(T));
  fin.read(reinterpret_cast<char *>(buffer.data()), size);
  return buffer;
}

#endif // INCLUDE_TOOLS_HPP_
