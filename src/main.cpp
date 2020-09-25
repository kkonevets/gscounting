// "Copyright 2020 Kirill Konevets"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>

#include "../include/tools.h"

using edge_t = EdgeItem<std::uint32_t>;

int main() {
  std::ifstream ifile("../data/edgelist.txt");
  std::ofstream ofile("../data/edgelist.bin", std::ios::binary);
  if (!ofile || !ifile) {
    std::cerr << "could not open file" << std::endl;
    return 1;
  }

  for (edge_t edge; ifile >> edge;) {
    if (!edge.encode(ofile)) {
      std::cerr << "could not encode edge" << std::endl;
      break;
    }
  }
  ofile.close();

  {
    std::ifstream fin("../data/edgelist.bin", std::ios::binary);
    for (auto edge = edge_t::decode(fin); edge.has_value();
         edge = edge_t::decode(fin)) {
      auto &val = edge.value();
      std::cout << val.first << " " << val.second << std::endl;
    }
  }

  return 0;
}
