// "Copyright 2020 Kirill Konevets"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

#include "../include/tools.h"

using EdgeItem = std::pair<std::uint32_t, std::uint32_t>;

namespace std {

std::istream &operator>>(std::istream &stream, EdgeItem &in) {
  return stream >> in.first >> in.second;
}

std::ostream &operator<<(std::ostream &stream, const EdgeItem &in) {
  return stream << "(" << in.first << ", " << in.second << ")";
}

} // namespace std

int main() {
  std::ifstream infile("../data/edgelist.txt");

  std::vector<EdgeItem> v{};
  std::copy(std::istream_iterator<EdgeItem>(infile), {}, std::back_inserter(v));

  std::copy(v.begin(), v.end(),
            std::ostream_iterator<EdgeItem>{std::cout, " "});

  return 0;
}
