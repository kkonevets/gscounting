// "Copyright 2020 Kirill Konevets"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

#include "externalsort.hpp"
#include "tools.hpp"

int main() {
  std::ifstream fin("tests/data/edgelist.bin", std::ios::binary);
  assert(fin);

  ExternalSorter<edge_type> sorter("tests/data", 4 * 4);
  auto merged = sorter.sort_unstable(fin);
  for (auto &item : merged) {
    std::cout << item << std::endl;
  }

  return 0;
}
