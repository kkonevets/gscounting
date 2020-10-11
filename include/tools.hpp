// "Copyright 2020 Kirill Konevets"

//!
//! @file tools.hpp
//! @brief Tools for efficient reading/writing of a graph
//!

#ifndef INCLUDE_TOOLS_HPP_
#define INCLUDE_TOOLS_HPP_

#include <algorithm>
#include <cassert>
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

using edge_type = EdgeItem<std::uint32_t>;
using adj_type = AdjItem<std::uint32_t>;

// ----------------------------------------------------------------------------
// Edge and adjacency items definition
// ----------------------------------------------------------------------------

/** @struct EdgeItem
 *
 *  @brief Structure representing a directed edge "first->second" of a graph
 *  @param First Source node id
 *  @param Second Target node id
 */
template <class T> struct EdgeItem : std::pair<T, T> {
  EdgeItem(T first, T second) : std::pair<T, T>(first, second) {}
  EdgeItem() : std::pair<T, T>(0, 0) {}

  auto encode(std::ostream &os) const -> bool {
    os.write(reinterpret_cast<const char *>(&(this->first)), sizeof(T));
    os.write(reinterpret_cast<const char *>(&(this->second)), sizeof(T));
    return os.good();
  }

  static auto decode(std::istream &is, EdgeItem<T> &edge) -> bool {
    is.read(reinterpret_cast<char *>(&edge.first), sizeof(T));
    is.read(reinterpret_cast<char *>(&edge.second), sizeof(T));
    return is.good();
  }
};

/** @struct AdjItem
 *
 *  @brief structure representing a record in adjacency list
 *  @param source Source node
 *  @param target Contains a list of target nodes
 */
template <class T> struct AdjItem {
  T source{};
  std::vector<T> targets;

  AdjItem(T source, std::vector<T> &&targets)
      : source(source), targets(std::move(targets)) {}
  AdjItem(AdjItem<T> &&other) noexcept
      : source(other.source), targets(std::move(other.targets)) {}
  AdjItem() = default;

  auto encode(std::ostream &os) const -> bool {
    T len{static_cast<T>(targets.size())};

    os.write(reinterpret_cast<const char *>(&len), sizeof(T));
    os.write(reinterpret_cast<const char *>(&source), sizeof(T));
    os.write(reinterpret_cast<const char *>(targets.data()),
             targets.size() * sizeof(T));
    return os.good();
  }

  static auto decode(std::istream &is, AdjItem<T> &row) -> bool {
    T len;
    is.read(reinterpret_cast<char *>(&len), sizeof(T));
    is.read(reinterpret_cast<char *>(&row.source), sizeof(T));

    row.targets.resize(len);
    is.read(reinterpret_cast<char *>(row.targets.data()), len * sizeof(T));
    return is.good();
  }
};

// ----------------------------------------------------------------------------
// Namespace std
// ----------------------------------------------------------------------------

namespace std {

template <class T>
auto operator>>(std::istream &is, EdgeItem<T> &edge) -> std::istream & {
  is >> edge.first >> edge.second;
  return is;
}

template <class T>
auto operator<<(std::ostream &os, EdgeItem<T> &edge) -> std::ostream & {
  os << "(" << edge.first << ", " << edge.second << ")";
  return os;
}

template <class T>
auto operator<<(std::ostream &os, const EdgeItem<T> &edge) -> std::ostream & {
  os << "(" << edge.first << ", " << edge.second << ")";
  return os;
}

template <class T>
auto operator>>(std::istream &is, AdjItem<T> &row) -> std::istream & {
  std::string line;
  std::getline(is, line);
  std::istringstream iss(line);
  iss >> row.source;
  row.targets.clear();
  std::copy(std::istream_iterator<T>(iss), {}, std::back_inserter(row.targets));
  return is;
}

template <class T>
auto operator<<(std::ostream &os, AdjItem<T> &row) -> std::ostream & {
  os << row.source << ": ";
  std::copy(row.targets.begin(), row.targets.end(),
            std::ostream_iterator<T>(os, " "));
  return os;
}

} // namespace std

// ----------------------------------------------------------------------------
// Misc
// ----------------------------------------------------------------------------

/**
 *  Read a vector from disk
 *
 *  @brief Loads vector<T> from binary file
 *  @param fname File name to load
 *  @return Vector<T>
 */
template <class T> auto read_vec(const std::string &fname) -> std::vector<T> {
  std::ifstream fin(fname, std::ios::binary | std::ios::ate);
  assert(fin);
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
