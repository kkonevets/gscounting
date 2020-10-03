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
  // std::vector<float> a(100);
  // std::iota(a.begin(), a.end(), 0);

  // auto v = ParallelApplyFoo(a);
  // for (auto &i : v) {
  //   std::cout << i << " ";
  // }
  // std::cout << std::endl;

  std::vector<std::uint32_t> indptr = {0, 2, 3, 6};
  std::vector<std::uint32_t> indices = {0, 2, 2, 0, 1, 2};
  std::vector<float> data = {1, 2, 3, 4, 5, 6};

  CSR m(std::move(data), std::move(indices), std::move(indptr));
  auto d{m.slice({0, 1, 2})};
  std::cout << d;

  return 0;
}
