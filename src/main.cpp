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
  std::vector<std::uint32_t> indptr = {0, 1, 1, 3};
  std::vector<std::uint32_t> indices = {0, 0, 1};
  std::vector<float> data = {1, 4, 5};

  CSR m(std::move(data), std::move(indices), std::move(indptr), 3, 3);

  // auto d{m.slice({0, 2})};
  // std::cout << d << std::endl;

  std::string fname("/Users/guyos/Documents/data/m.bin");
  m.save(fname);
  auto m_loaded{CSR::load(fname)};

  assert(m == m_loaded);

  return 0;
}
