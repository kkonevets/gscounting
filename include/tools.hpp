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

template <class T> struct EdgeItem;
template <class T> struct AdjItem;

using edge_t = EdgeItem<std::uint32_t>;
using adj_t = AdjItem<std::uint32_t>;

// ----------------------------------------------------------------------------
// Edge and adjacency items definition
// ----------------------------------------------------------------------------

/** @struct EdgeItem
 *
 *  @brief structure representiong a directed edge "source->target" in a graph
 */
template <class T> struct EdgeItem {
  T source, target;

  explicit EdgeItem(T source, T target) : source(source), target(target) {}
  EdgeItem() : source{0}, target{0} {}

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
 *
 *  @brief structure representing a record in adjacency list
 *
 *  k is a source node and v contains a list of target nodes.
 */
template <class T> struct AdjItem {
  T k;
  std::vector<T> v;

  explicit AdjItem(T k, std::vector<T> &&v) : k(k), v(v) {}
  AdjItem() : k{0} {}

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

class ListIteratorSentinel {};

template <class T> class ListIterator {
  std::istream &is;
  T curr;
  bool good;

public:
  explicit ListIterator(std::istream &is) : is{is} {
    good = T::decode(is, curr);
  }

  T &operator*() { return curr; }

  ListIterator &operator++() {
    good = T::decode(is, curr);
    return *this;
  }

  bool operator!=(const ListIteratorSentinel) const { return good; }
};

/** @class list_range
 *
 *  @brief iteration on edge and adjacency lists
 *
 *  T could be EdgeItem or AdjItem.
 */
template <class T> class list_range {
  std::istream &is;

public:
  explicit list_range(std::istream &is) : is{is} {}

  ListIterator<T> begin() const { return ListIterator<T>{is}; }
  ListIteratorSentinel end() const { return {}; }
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
  fin.exceptions(std::ifstream::failbit);
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
