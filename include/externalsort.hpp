// "Copyright 2020 Kirill Konevets"

#ifndef INCLUDE_EXTERNALSORT_HPP_
#define INCLUDE_EXTERNALSORT_HPP_

#include <bitset>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <iterator>
#include <math.h>
#include <string>
#include <vector>

#include "tools.hpp"

namespace fs = std::filesystem;

template <class T> class ExternalSorter {
  std::size_t max_mem;
  const fs::path &save_dir;
  unsigned int file_count;

  void write_chunk(const std::vector<T> &buf) {
    std::ofstream ofile(save_dir, std::ios::binary);
    ofile.write(reinterpret_cast<char *>(buf.data()), buf.size() * sizeof(T));
    file_count += 1;
  }

public:
  explicit ExternalSorter(std::iterator<std::input_iterator_tag, T> it,
                          const fs::path &save_dir,
                          std::size_t max_mem = pow(2, 30))
      : save_dir(save_dir), max_mem(max_mem), file_count(0) {}
};

#endif // INCLUDE_EXTERNALSORT_HPP_
