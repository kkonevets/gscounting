#include "tools.hpp"

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

// ----------------------------------------------------------------------------
// Edge and adjacency items definition
// ----------------------------------------------------------------------------

template <class T> auto EdgeItem<T>::encode(std::ostream &os) const -> bool {
  os.write(reinterpret_cast<const char *>(&(this->first)), sizeof(T));
  os.write(reinterpret_cast<const char *>(&(this->second)), sizeof(T));
  return os.good();
}

template <class T>
auto EdgeItem<T>::decode(std::istream &is, EdgeItem<T> &edge) -> bool {
  is.read(reinterpret_cast<char *>(&edge.first), sizeof(T));
  is.read(reinterpret_cast<char *>(&edge.second), sizeof(T));
  return is.good();
}

auto operator>>(std::istream &is, EdgeItem<std::uint32_t> &edge)
    -> std::istream & {
  is >> edge.first >> edge.second;
  return is;
}

auto operator<<(std::ostream &os, const EdgeItem<std::uint32_t> &edge)
    -> std::ostream & {
  os << "(" << edge.first << ", " << edge.second << ")";
  return os;
}

template <class T> auto AdjItem<T>::encode(std::ostream &os) const -> bool {
  T len{static_cast<T>(targets.size())};

  os.write(reinterpret_cast<const char *>(&len), sizeof(T));
  os.write(reinterpret_cast<const char *>(&source), sizeof(T));
  os.write(reinterpret_cast<const char *>(targets.data()),
           targets.size() * sizeof(T));
  return os.good();
}

template <class T>
auto AdjItem<T>::decode(std::istream &is, AdjItem<T> &row) -> bool {
  T len;
  is.read(reinterpret_cast<char *>(&len), sizeof(T));
  is.read(reinterpret_cast<char *>(&row.source), sizeof(T));

  row.targets.resize(len);
  is.read(reinterpret_cast<char *>(row.targets.data()), len * sizeof(T));
  return is.good();
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
auto operator<<(std::ostream &os, const AdjItem<T> &row) -> std::ostream & {
  os << row.source << ": ";
  std::copy(row.targets.begin(), row.targets.end(),
            std::ostream_iterator<T>(os, " "));
  return os;
}

// Explicit template instantiation
template struct EdgeItem<std::uint32_t>;
template struct AdjItem<std::uint32_t>;

// ----------------------------------------------------------------------------
// Misc
// ----------------------------------------------------------------------------

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
