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

  ExternalSorter sorter(list_range<edge_t>(fin), "tests/data", 4 * 4);
  sorter.sort_unstable();

  return 0;
}
