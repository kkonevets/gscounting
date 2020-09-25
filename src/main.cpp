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
using adj_t = AdjItem<std::uint32_t>;

void test_edge() {
  {
    std::ifstream ifile("../data/edgelist.txt");
    std::ofstream ofile("../data/edgelist.bin", std::ios::binary);
    if (!ofile || !ifile) {
      std::cerr << "could not open file" << std::endl;
      return;
    }

    for (edge_t edge; ifile >> edge;) {
      if (!edge.encode(ofile)) {
        std::cerr << "could not encode edge" << std::endl;
        break;
      }
    }
  }

  {
    std::ifstream fin("../data/edgelist.bin", std::ios::binary);
    for (edge_t edge; edge_t::decode(fin, edge);) {
      std::cout << edge.first << " " << edge.second << std::endl;
    }
  }
}

int main() {
  {
    std::ofstream ofile("../data/edjlist.bin", std::ios::binary);
    if (!ofile) {
      std::cerr << "could not open file" << std::endl;
      return 1;
    }

    ofile.unsetf(std::ios::skipws);
    adj_t row{3, {1, 2, 3, 4, 5}};
    if (!row.encode(ofile)) {
      std::cerr << "could not encode row" << std::endl;
    }
  }

  {
    std::ifstream fin("../data/edjlist.bin", std::ios::binary);
    for (adj_t row; adj_t::decode(fin, row);) {
      std::cout << "k: " << row.k << std::endl;
      for (auto &j : row.v) {
        std::cout << j << " ";
      }
      std::cout << std::endl;
    }
  }

  return 0;
}
