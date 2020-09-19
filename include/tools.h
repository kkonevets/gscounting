// "Copyright 2020 Kirill Konevets"

#ifndef _GSC_INCLUDE_TOOLS_H
#define _GSC_INCLUDE_TOOLS_H

#include <cstdint>
#include <fstream>
#include <iterator>
#include <optional>
#include <utility>
#include <vector>

template <class T> struct EdgeItem {
  T first, second;
  explicit EdgeItem(T first, T second) : first(first), second(second) {}
  bool encode(std::ostream &os) {
    os.write(reinterpret_cast<char *>(&first), sizeof(T));
    os.write(reinterpret_cast<char *>(&second), sizeof(T));
    if (os)
      return true;
    else
      return true;
  }
  static std::optional<EdgeItem<T>> decode(std::istream &is) {
    T first, second;
    is.read(reinterpret_cast<char *>(&first), sizeof(T));
    is.read(reinterpret_cast<char *>(&second), sizeof(T));
    if (is)
      return {EdgeItem<T>{first, second}};
    else
      return {};
  }
};

#endif // _GSC_INCLUDE_TOOLS_H
