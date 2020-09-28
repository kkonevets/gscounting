// "Copyright 2020 Kirill Konevets"

#ifndef INCLUDE_EXTERNALSORT_HPP_
#define INCLUDE_EXTERNALSORT_HPP_

#include <math.h>

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <istream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include "tools.hpp"

namespace fs = std::filesystem;

template <class InputIt> class ExternalSorter {
  using T = typename std::iterator_traits<InputIt>::value_type;

  InputIt iter;
  const fs::path &save_dir;
  std::size_t max_mem;
  unsigned int nChunks;

  void sort_save(std::vector<T> &buf) {
    std::sort(buf.begin(), buf.end());
    auto fout = save_dir / (std::to_string(nChunks) + ".bin");
    std::ofstream ofile(fout, std::ios::binary);
    assert(ofile);

    // std::cout << save_dir / std::to_string(nChunks) << std::endl;
    for (auto &item : buf) {
      item.encode(ofile);
    }
    nChunks += 1;
    return;
  }

public:
  explicit ExternalSorter(InputIt iter, const fs::path &save_dir,
                          std::size_t max_mem = pow(2, 30))
      : iter(iter), save_dir(save_dir), max_mem(std::max(max_mem, sizeof(T))),
        nChunks(0) {}
  void sort_unstable() {
    auto max_size = max_mem / sizeof(T);
    std::vector<T> buf;
    buf.reserve(max_size);
    for (auto &item : iter) {
      buf.push_back(std::move(item));
      if (buf.size() == max_size) {
        sort_save(buf);
        buf.clear();
      }
    }
    if (!buf.empty()) {
      sort_save(buf);
    }

    std::vector<T>().swap(buf); // free memory

    std::vector<InputIt> readers;
    for (size_t i = 0; i < nChunks; ++i) {
      std::ifstream is(save_dir, std::ios::binary);
      assert(is);
      readers.push_back(list_range<T>(is));
    }
  }
};

#endif // INCLUDE_EXTERNALSORT_HPP_
