// "Copyright 2020 Kirill Konevets"

//!
//! @file tools.hpp
//! @brief Tools for efficient reading/writing of a graph
//!

#ifndef INCLUDE_TOOLS_HPP_
#define INCLUDE_TOOLS_HPP_

#include <vector>

/** @struct EdgeItem
 *
 *  @brief Structure representing a directed edge "first->second" of a graph
 *  @param First Source node id
 *  @param Second Target node id
 */
template <class T> struct EdgeItem : std::pair<T, T> {
  EdgeItem(T first, T second) : std::pair<T, T>(first, second) {}
  EdgeItem() : std::pair<T, T>(0, 0) {}

  auto encode(std::ostream &os) const -> bool;

  static auto decode(std::istream &is, EdgeItem<T> &edge) -> bool;

  friend auto operator>>(std::istream &is, EdgeItem<T> &edge) -> std::istream &;

  friend auto operator<<(std::ostream &os, const EdgeItem<T> &edge)
      -> std::ostream &;
};

/** @struct AdjItem
 *
 *  @brief structure representing a record in adjacency list
 *  @param source Source node
 *  @param target Contains a list of target nodes
 */
template <class T> struct AdjItem {
  T source{};
  std::vector<T> targets;

  AdjItem(T source, std::vector<T> &&targets)
      : source(source), targets(std::move(targets)) {}
  AdjItem(AdjItem<T> &&other) noexcept
      : source(other.source), targets(std::move(other.targets)) {}
  AdjItem() = default;

  auto encode(std::ostream &os) const -> bool;

  static auto decode(std::istream &is, AdjItem<T> &row) -> bool;

  friend auto operator>>(std::istream &is, AdjItem<T> &row) -> std::istream &;

  friend auto operator<<(std::ostream &os, const AdjItem<T> &row)
      -> std::ostream &;
};

// ----------------------------------------------------------------------------
// Misc
// ----------------------------------------------------------------------------

/**
 *  Read a vector from disk
 *
 *  @brief Loads vector<T> from binary file
 *  @param fname File name to loadp
 *  @return Vector<T>
 */
template <class T> auto read_vec(const std::string &fname) -> std::vector<T>;

#endif // INCLUDE_TOOLS_HPP_
