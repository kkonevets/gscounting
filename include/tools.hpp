// "Copyright 2020 Kirill Konevets"

#ifndef INCLUDE_TOOLS_HPP_
#define INCLUDE_TOOLS_HPP_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <istream>
#include <iterator>
#include <memory>
#include <sstream>
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
template <class T> struct EdgeItem : std::pair<T, T> {
  EdgeItem(T source, T target) : std::pair<T, T>(source, target) {}
  EdgeItem() : std::pair<T, T>(0, 0) {}

  bool encode(std::ostream &os) {
    os.write(reinterpret_cast<char *>(&(this->first)), sizeof(T));
    os.write(reinterpret_cast<char *>(&(this->second)), sizeof(T));
    return os.good();
  }

  static bool decode(std::istream &is, EdgeItem<T> &edge) {
    is.read(reinterpret_cast<char *>(&edge.first), sizeof(T));
    is.read(reinterpret_cast<char *>(&edge.second), sizeof(T));
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
  T source;
  std::vector<T> targets;

  AdjItem(T source, std::vector<T> &&targets)
      : source(source), targets(std::move(targets)) {}
  AdjItem(AdjItem<T> &&other)
      : source(other.source), targets(std::move(other.targets)) {}
  AdjItem() : source{0} {}

  bool encode(std::ostream &os) const {
    T len{static_cast<T>(targets.size())};

    os.write(reinterpret_cast<const char *>(&len), sizeof(T));
    os.write(reinterpret_cast<const char *>(&source), sizeof(T));
    os.write(reinterpret_cast<const char *>(targets.data()),
             targets.size() * sizeof(T));
    return os.good();
  }

  static bool decode(std::istream &is, AdjItem<T> &row) {
    T len;
    is.read(reinterpret_cast<char *>(&len), sizeof(T));
    is.read(reinterpret_cast<char *>(&row.source), sizeof(T));

    row.targets.resize(len);
    is.read(reinterpret_cast<char *>(row.targets.data()), len * sizeof(T));
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

template <class T> struct iterator_traits<list_range<T>> {
public:
  using iterator_category = std::input_iterator_tag;
  using value_type = T;
};

template <class T>
std::istream &operator>>(std::istream &is, EdgeItem<T> &edge) {
  is >> edge.first >> edge.second;
  return is;
}

template <class T>
std::ostream &operator<<(std::ostream &os, EdgeItem<T> &edge) {
  os << edge.first << " " << edge.second << std::endl;
  return os;
}

template <class T> std::istream &operator>>(std::istream &is, AdjItem<T> &row) {
  std::string line;
  std::getline(is, line);
  std::istringstream iss(line);
  iss >> row.source;
  row.targets.clear();
  std::copy(std::istream_iterator<T>(iss), {}, std::back_inserter(row.targets));
  return is;
}

template <class T> std::ostream &operator<<(std::ostream &os, AdjItem<T> &row) {
  os << row.source << ": ";
  for (auto &i : row.targets) {
    os << i << " ";
  }
  return os;
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
