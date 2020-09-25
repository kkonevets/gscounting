// "Copyright 2020 Kirill Konevets"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

#include "../include/tools.h"

using edge_t = EdgeItem<std::uint32_t>;

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
    edge_t edge(1, 2);
    if (!edge.encode(ofile)) {
      std::cerr << "could not encode edge" << std::endl;
    }
  }

  {
    std::ifstream fin("../data/edgelist.bin", std::ios::in | std::ios::binary);
    for (auto edge = edge_t::decode(fin); edge.has_value();
         edge = edge_t::decode(fin)) {
      auto &val = edge.value();
      std::cout << val.first << "," << val.second << std::endl;
    }
  }

  return 0;
}
