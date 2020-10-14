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
  auto m = CSR::random(5, 5, 0.5);
  auto d = m.slice({0, 1, 2, 3, 4});
  std::cout << d << std::endl;
  return 0;
}
