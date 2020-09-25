// "Copyright 2020 Kirill Konevets"

#ifndef _GSC_INCLUDE_TOOLS_H
#define _GSC_INCLUDE_TOOLS_H

#include <fstream>
#include <optional>

template <class T> struct EdgeItem {
  T first, second;

  explicit EdgeItem(T first, T second) : first(first), second(second) {}
  EdgeItem() : first(0), second(0) {}

  bool encode(std::ostream &os) {
    os.write(reinterpret_cast<char *>(&first), sizeof(T));
    os.write(reinterpret_cast<char *>(&second), sizeof(T));
    return os.good();
  }

  static std::optional<EdgeItem<T>> decode(std::istream &is) {
    T first, second;
    is.read(reinterpret_cast<char *>(&first), sizeof(T));
    is.read(reinterpret_cast<char *>(&second), sizeof(T));
    if (is)
      return EdgeItem<T>{first, second};
    else
      return {};
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
