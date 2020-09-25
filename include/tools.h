// "Copyright 2020 Kirill Konevets"

#ifndef _GSC_INCLUDE_TOOLS_H
#define _GSC_INCLUDE_TOOLS_H

#include <fstream>
#include <iostream>
#include <iterator>
#include <utility>
#include <vector>

template <class T> struct EdgeItem {
  T first, second;

  explicit EdgeItem(T first, T second) : first(first), second(second) {}
  EdgeItem() {}

  bool encode(std::ostream &os) {
    os.write(reinterpret_cast<char *>(&first), sizeof(T));
    os.write(reinterpret_cast<char *>(&second), sizeof(T));
    return os.good();
  }

  static bool decode(std::istream &is, EdgeItem<T> &edge) {
    is.read(reinterpret_cast<char *>(&edge.first), sizeof(T));
    is.read(reinterpret_cast<char *>(&edge.second), sizeof(T));
    return is.good();
  }
};

template <class T> struct AdjItem {
  T k;
  std::vector<T> v;

  explicit AdjItem(T k, std::vector<T> &&v) : k(k), v(v) {}
  AdjItem() : k(0) {}

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

namespace std {

template <class T>
std::istream &operator>>(std::istream &is, EdgeItem<T> &edge) {
  is >> edge.first >> edge.second;
  return is;
}
} // namespace std

#endif // _GSC_INCLUDE_TOOLS_H
