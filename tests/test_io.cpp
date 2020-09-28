// "Copyright 2020 Kirill Konevets"
#include "gtest/gtest.h"
#include <sys/_types/_size_t.h>

#include "tools.hpp"

TEST(IteratorTest, Edge) {
  auto wrong_bit = std::ifstream::failbit | std::ifstream::badbit;
  std::vector<edge_t> edges;
  {
    std::ifstream ifile("tests/data/edgelist.txt", wrong_bit);
    std::ofstream ofile("tests/data/edgelist.bin",
                        wrong_bit | std::ios::binary);

    for (edge_t edge; ifile >> edge;) {
      EXPECT_TRUE(edge.encode(ofile));
      edges.push_back(edge);
    }
  }

  {
    std::ifstream fin("tests/data/edgelist.bin", wrong_bit | std::ios::binary);
    size_t i = 0;
    for (auto &edge : list_range<edge_t>(fin)) {
      auto &_edge = edges[i];
      EXPECT_EQ(_edge.source, edge.source);
      EXPECT_EQ(_edge.target, edge.target);
      i++;
    }
  }
}

TEST(IteratorTest, Adjacency) {
  auto bits = std::ios::binary | std::ifstream::failbit | std::ifstream::badbit;
  adj_t row{3, {1, 2, 3, 4, 5}};
  {
    std::ofstream ofile("tests/data/edjlist.bin", bits);

    ofile.unsetf(std::ios::skipws);
    EXPECT_TRUE(row.encode(ofile));
  }

  {
    std::ifstream fin("tests/data/edjlist.bin", bits);
    for (auto &cur_row : list_range<adj_t>(fin)) {
      EXPECT_EQ(cur_row.k, row.k);
      for (size_t i = 0; i < cur_row.v.size(); i++) {
        EXPECT_EQ(cur_row.v[i], row.v[i]);
      }
    }
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
