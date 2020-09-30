// "Copyright 2020 Kirill Konevets"

#include <cstddef>
#include <filesystem>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "externalsort.hpp"
#include "tools.hpp"
#include "gtest/gtest.h"

namespace fs = std::filesystem;

constexpr size_t EDGE_LIST_LENGTH{100};

fs::path pjoin(std::string fname) { return fs::path("./tests/data/") / fname; }

TEST(IteratorTest, Edge) {
  std::vector<edge_type> edges;
  {
    std::ifstream ifile(pjoin("edgelist.txt"));
    std::ofstream ofile(pjoin("edgelist.bin"), std::ios::binary);
    ASSERT_TRUE(ifile && ofile);

    for (edge_type edge; ifile >> edge;) {
      EXPECT_TRUE(edge.encode(ofile));
      edges.push_back(edge);
    }
  }

  {
    std::ifstream fin(pjoin("edgelist.bin"), std::ios::binary);
    ASSERT_TRUE(fin);

    size_t i = 0;
    for (edge_type edge; edge_type::decode(fin, edge);) {
      auto &_edge = edges[i];
      EXPECT_EQ(_edge.first, edge.first);
      EXPECT_EQ(_edge.second, edge.second);
      i++;
    }
  }
}

TEST(IteratorTest, Adjacency) {
  adj_type row{3, {1, 2, 3, 4, 5}};
  {
    std::ofstream ofile(pjoin("edjlist.bin"), std::ios::binary);
    ASSERT_TRUE(ofile);

    ofile.unsetf(std::ios::skipws);
    EXPECT_TRUE(row.encode(ofile));
  }

  {
    std::ifstream fin(pjoin("edjlist.bin"), std::ios::binary);
    ASSERT_TRUE(fin);

    for (adj_type cur_row; adj_type::decode(fin, cur_row);) {
      EXPECT_EQ(cur_row.source, row.source);
      for (size_t i = 0; i < cur_row.targets.size(); i++) {
        EXPECT_EQ(cur_row.targets[i], row.targets[i]);
      }
    }
  }
}

TEST(ExternalSorterTest, GenerateEdges) {
  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_int_distribution<int> uni(0, 1000000); // guaranteed unbiased

  std::ofstream os(pjoin("edgelist_big.bin"), std::ios::binary);
  ASSERT_TRUE(os);

  for (size_t i = 0; i < EDGE_LIST_LENGTH; ++i) {
    EdgeItem(uni(rng), uni(rng)).encode(os);
  }
}

TEST(ExternalSorterTest, SortUnstableAndSave) {
  std::ifstream fin(pjoin("edgelist_big.bin"), std::ios::binary);
  ASSERT_TRUE(fin);

  size_t max_mem = EDGE_LIST_LENGTH * sizeof(edge_type) / 5;
  ExternalSorter<edge_type> sorter("tests/data", max_mem);
  auto merged = sorter.sort_unstable(fin);

  std::ofstream os(pjoin("edgelist_big_sorted.bin"), std::ios::binary);
  ASSERT_TRUE(os);

  for (auto &item : merged) {
    item.encode(os);
  }
}

TEST(ExternalSorterTest, CheckEqual) {
  std::ifstream fin(pjoin("edgelist_big.bin"), std::ios::binary);
  ASSERT_TRUE(fin);

  std::vector<edge_type> v;
  for (edge_type edge; edge_type::decode(fin, edge);) {
    v.push_back(edge);
  }
  std::sort(v.begin(), v.end());

  std::ifstream fin_sorted(pjoin("edgelist_big_sorted.bin"), std::ios::binary);
  ASSERT_TRUE(fin_sorted);

  edge_type edge2;
  for (size_t i = 0; i < v.size(); ++i) {
    auto &edge1 = v.at(i);
    ASSERT_TRUE(edge_type::decode(fin_sorted, edge2));
    ASSERT_EQ(edge1, edge2);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
