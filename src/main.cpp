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

#include "csr_matrix.hpp"
#include "externalsort.hpp"
#include "tools.hpp"

int main() {
  std::vector<float> a(100);
  std::iota(a.begin(), a.end(), 0);

  auto v = ParallelApplyFoo(a);
  for (auto &i : v) {
    std::cout << i << " ";
  }
  std::cout << std::endl;

  return 0;
}
