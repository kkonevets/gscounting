// "Copyright 2020 Kirill Konevets"

//!
//! @file externalsort.hpp
//! Sorting data that does not fit into memory
//!

#ifndef INCLUDE_EXTERNALSORT_HPP_
#define INCLUDE_EXTERNALSORT_HPP_

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <execution>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>
#include <string>
#include <utility>
#include <vector>

#include "tools.hpp"

namespace fs = std::filesystem;

// ----------------------------------------------------------------------------
// KMergeIterator
// ----------------------------------------------------------------------------

/** @class KmergeIteratorSentinel
 *
 * Helper class indicating end of stream
 */
class KmergeIteratorSentinel {};

/** @class KMergeIterator
 *
 * An iterator doing actual work for merging files using priority queue.
 * @param readers File streams to merge
 */
template <class T> class KMergeIterator {
  std::vector<std::ifstream> &readers;
  bool good;
  using _t = typename std::pair<T, size_t>;
  static constexpr auto cmp = [](const _t &l, const _t &r) {
    return l.first > r.first;
  };
  std::priority_queue<_t, std::vector<_t>, decltype(cmp)> q{cmp};
  T temp;

  auto init_queue() -> bool {
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

  auto operator*() -> const T & { return q.top().first; }

  auto operator++() -> KMergeIterator & {
    auto i = q.top().second;
    auto &is = readers.at(i);
    q.pop();
    if (T::decode(is, temp)) {
      q.emplace(temp, i); // COPY temp, it's cheap
    }

    good = !q.empty() || !init_queue();

    return *this;
  }

  auto operator!=(const KmergeIteratorSentinel /*unused*/) const -> bool {
    return good;
  }
};

/** @class KMerge
 *
 * Iterator that merges multiple sorted iterators. Uses priority queue
 * for merging
 */
template <class T> class KMerge {
  std::vector<std::ifstream> readers;

public:
  explicit KMerge(std::vector<std::ifstream> &&readers)
      : readers{std::move(readers)} {}

  auto begin() -> KMergeIterator<T> { return KMergeIterator<T>{readers}; }
  auto end() -> KmergeIteratorSentinel { return {}; }
};

// ----------------------------------------------------------------------------
// ExternalSorter
// ----------------------------------------------------------------------------

/** @class ExternalSorter
 *
 *  Sorts file on disk using merge sort.
 *  First it splits file on parts, then sorts them (consuming `max_mem` at a
 *  time), saves parts on disk to `save_dir` and then merges those parts while
 *  lazy loading. Uses priority queue for merging (memory consumption is
 *  minimal).
 *
 *  @param save_dir Name of a directory to save parts in
 *  @param max_mem Maximum size of a part file in bytes, 1073741824 (1Gb) by
 * default. The more memory is available the faster is the sorting
 */
template <class T> class ExternalSorter {
  const fs::path save_dir;
  std::size_t max_mem;
  unsigned int nChunks;

  auto file_name(unsigned int n) -> fs::path {
    return save_dir / (std::to_string(n) + ".bin");
  }

  void sort_save(std::vector<T> &buf) {
#ifdef __clang__
    std::sort(buf.begin(), buf.end());
#else
    std::sort(std::execution::par_unseq, buf.begin(), buf.end());
#endif

    auto fout = file_name(nChunks);
    std::ofstream ofile(fout, std::ios::binary);
    assert(ofile);

    for (auto &item : buf) {
      item.encode(ofile);
    }
    nChunks += 1;
  }

public:
  explicit ExternalSorter(fs::path save_dir, size_t max_mem = pow(2, 30))
      : save_dir(std::move(save_dir)), max_mem(std::max(max_mem, sizeof(T))),
        nChunks(0) {}
  /** @fn sort_unstable
   *
   *  @brief Sorts input stream
   *  @param is Input stream (e.g. file)
   *  @return A merging iterator that lazily loads data from sorted files
   */
  auto sort_unstable(std::istream &is) -> KMerge<T> {
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
