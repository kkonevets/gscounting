// "Copyright 2020 Kirill Konevets"

#ifndef INCLUDE_EXTERNALSORT_HPP_
#define INCLUDE_EXTERNALSORT_HPP_

#include <cstddef>
#include <cstdio>
#include <iostream>
#include <math.h>

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <numeric>
#include <queue>
#include <string>
#include <type_traits>
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
  T temp;

  bool init_queue() {
    for (size_t i = 0; i < readers.size(); ++i) {
      auto &is = readers.at(i);
      if (T::decode(is, temp)) {
        q.emplace(temp, i);
      }
    }

    return q.empty();
  }

public:
  explicit KMergeIterator(std::vector<std::ifstream> &readers)
      : readers{readers} {
    good = !init_queue();
  }

  const T &operator*() { return q.top().first; }

  KMergeIterator &operator++() {
    auto i = q.top().second;
    auto &is = readers.at(i);
    q.pop();
    if (T::decode(is, temp)) {
      q.emplace(temp, i); // COPY temp, it's cheap
    }

    good = !q.empty() || !init_queue();

    if (!good) {
      // std::cout << remain.size() << std::endl;
    }

    return *this;
  }

  bool operator!=(const KmergeIteratorSentinel) const { return good; }
};

/** @class KMerge
 *
 *  @brief Iterator that merges multiple sorted iterators. Uses priority queue
 * for merging
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
 *  @brief Sorts file on disk using merge sort.
 *  First it splits file on parts, then sorts them (consuming `max_mem` at a
 * time), saves parts on disk to `save_dir` and then merges those parts while
 * lazy loading. Uses priority queue for merging (memory consumption is
 * minimal).
 *
 *  @param save_dir Name of a directory to save parts in
 *  @param max_mem Maximum size of a part file in bytes, 1073741824 (1Gb) by
 * default. The more memory is available the faster is the sorting
 */
template <class T> class ExternalSorter {
  const fs::path &save_dir;
  std::size_t max_mem;
  unsigned int nChunks;

  fs::path file_name(unsigned int n) {
    return save_dir / (std::to_string(n) + ".bin");
  }

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

public:
  explicit ExternalSorter(const fs::path &save_dir, size_t max_mem = pow(2, 30))
      : save_dir(save_dir), max_mem(std::max(max_mem, sizeof(T))), nChunks(0) {}
  /** @fn sort_unstable
   *
   *  @brief Sorts input stream
   *  @param is Input stream (e.g. file)
   *  @return A merging iterator that lazily loads data from sorted files
   */
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
      readers.emplace_back(file_name(i), std::ios::binary);
      assert(readers.back()); // check io errors
    }

    return KMerge<T>(std::move(readers));
  }
};

#endif // INCLUDE_EXTERNALSORT_HPP_
