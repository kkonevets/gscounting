// "Copyright 2020 Kirill Konevets"

#ifndef INCLUDE_EXTERNALSORT_HPP_
#define INCLUDE_EXTERNALSORT_HPP_

#include <math.h>

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <istream>
#include <iterator>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include "tools.hpp"

namespace fs = std::filesystem;

// ----------------------------------------------------------------------------
// KMergeIterator
// ----------------------------------------------------------------------------

class KmergeIteratorSentinel {};

template <class T> class KMergeIterator {
  std::vector<std::ifstream> &readers;
  bool good;
  using _t = typename std::pair<T, size_t>;
  static constexpr auto cmp = [](const _t &l, const _t &r) {
    return l.first > r.first;
  };
  std::priority_queue<_t, std::vector<_t>, decltype(cmp)> q{cmp};
  T cur;

  void init_queue() {
    size_t i = 0;
    for (auto &is : readers) {
      if (T item; T::decode(is, item)) {
        q.emplace(item, i);
      }
      i++;
    }
  }

public:
  explicit KMergeIterator(std::vector<std::ifstream> &readers)
      : readers{readers} {
    init_queue();
    good = !q.empty();
  }

  const T &operator*() { return q.top().first; }

  KMergeIterator &operator++() {
    auto &[_, i] = q.top();
    std::ifstream &is = readers[i];
    q.pop();
    if (T::decode(is, cur)) {
      q.emplace(cur, i); // COPY
    }

    good = !q.empty();

    return *this;
  }

  bool operator!=(const KmergeIteratorSentinel) const { return good; }
};

/** @class KMerge
 *
 *  @brief Iterator that merges multiple sorted iterators
 */
template <class T> class KMerge {
  std::vector<std::ifstream> readers;

public:
  explicit KMerge(std::vector<std::ifstream> &&readers)
      : readers{std::move(readers)} {}

  KMergeIterator<T> begin() { return KMergeIterator<T>{readers}; }
  KmergeIteratorSentinel end() { return {}; }
};

// ----------------------------------------------------------------------------
// ExternalSorter
// ----------------------------------------------------------------------------

/** @class ExternalSorter
 *
 *  @brief Sorts file on disk using merge sort
 */
template <class T> class ExternalSorter {
  const fs::path &save_dir;
  std::size_t max_mem;
  unsigned int nChunks;

  void sort_save(std::vector<T> &buf) {
    std::sort(buf.begin(), buf.end());
    auto fout = file_name(nChunks);
    std::ofstream ofile(fout, std::ios::binary);
    assert(ofile);

    for (auto &item : buf) {
      item.encode(ofile);
    }
    nChunks += 1;
    return;
  }

  fs::path file_name(unsigned int n) {
    return save_dir / (std::to_string(n) + ".bin");
  }

public:
  explicit ExternalSorter(const fs::path &save_dir,
                          std::size_t max_mem = pow(2, 30))
      : save_dir(save_dir), max_mem(std::max(max_mem, sizeof(T))), nChunks(0) {}
  KMerge<T> sort_unstable(std::istream &is) {
    auto max_size = max_mem / sizeof(T);
    std::vector<T> buf;
    buf.reserve(max_size);
    for (T item; T::decode(is, item);) {
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

    std::vector<std::ifstream> readers;
    for (size_t i = 0; i < nChunks; ++i) {
      std::ifstream is(file_name(i), std::ios::binary);
      assert(is);
      readers.push_back(std::move(is));
    }

    return KMerge<T>(std::move(readers));
  }
};

#endif // INCLUDE_EXTERNALSORT_HPP_
