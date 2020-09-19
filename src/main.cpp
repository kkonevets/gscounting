// "Copyright 2020 Kirill Konevets"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

#include "../include/tools.h"

// using EdgeItem = std::pair<std::uint32_t, std::uint32_t>;

// namespace std {

// std::istream &operator>>(std::istream &stream, EdgeItem &in) {
//   return stream >> in.first >> in.second;
// }

// std::ostream &operator<<(std::ostream &stream, const EdgeItem &in) {
//   return stream << "(" << in.first << ", " << in.second << ")";
// }

// } // namespace std

int main() {
  // std::ifstream infile("../data/edgelist.txt");

  // std::vector<EdgeItem> v{};
  // std::copy(std::istream_iterator<EdgeItem>(infile), {},
  // std::back_inserter(v));

  // std::copy(v.begin(), v.end(),
  //           std::ostream_iterator<EdgeItem>{std::cout, " "});

  {
    std::ofstream ofile("../data/edgelist.bin",
                        std::ios::out | std::ios::binary);
    if (!ofile) {
      std::cerr << "could not open file" << std::endl;
      return 1;
    }
    EdgeItem<std::uint32_t> edge(1, 2);
    if (!edge.encode(ofile)) {
      std::cerr << "could not encode edge" << std::endl;
    }
  }

  {
    std::ifstream infile("../data/edgelist.bin",
                         std::ios::in | std::ios::binary);
    auto edge = EdgeItem<std::uint32_t>::decode(infile);
    auto &val = edge.value();
    std::cout << val.first << "," << val.second << std::endl;
  }

  return 0;
}
