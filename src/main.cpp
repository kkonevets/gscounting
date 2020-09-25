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
    for (auto edge = edge_t::decode(fin); edge.has_value();
         edge = edge_t::decode(fin)) {
      auto &val = edge.value();
      std::cout << val.first << " " << val.second << std::endl;
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
    for (auto row = adj_t::decode(fin); row.has_value();
         row = adj_t::decode(fin)) {
      auto &val = row.value();
      std::cout << "k: " << val.k << std::endl;

      std::copy(val.v.begin(), val.v.end(),
                std::ostream_iterator<decltype(adj_t::k)>(std::cout, " "));
      std::cout << std::endl;
    }
  }

  return 0;
}
