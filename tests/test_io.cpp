// "Copyright 2020 Kirill Konevets"
#include "gtest/gtest.h"
#include <iostream>

#include "tools.hpp"

TEST(IteratorTest, Edge) {
  std::vector<edge_t> edges;
  {
    std::ifstream ifile("./tests/data/edgelist.txt");
    std::ofstream ofile("./tests/data/edgelist.bin", std::ios::binary);
    ASSERT_TRUE(ifile && ofile);

    for (edge_t edge; ifile >> edge;) {
      EXPECT_TRUE(edge.encode(ofile));
      edges.push_back(edge);
    }
  }

  {
    std::ifstream fin("./tests/data/edgelist.bin", std::ios::binary);
    ASSERT_TRUE(fin);

    size_t i = 0;
    for (auto &edge : list_range<edge_t>(fin)) {
      auto &_edge = edges[i];
      EXPECT_EQ(_edge.first, edge.first);
      EXPECT_EQ(_edge.second, edge.second);
      i++;
    }
  }
}

TEST(IteratorTest, Adjacency) {
  adj_t row{3, {1, 2, 3, 4, 5}};
  {
    std::ofstream ofile("./tests/data/edjlist.bin", std::ios::binary);
    ASSERT_TRUE(ofile);

    ofile.unsetf(std::ios::skipws);
    EXPECT_TRUE(row.encode(ofile));
  }

  {
    std::ifstream fin("./tests/data/edjlist.bin", std::ios::binary);
    ASSERT_TRUE(fin);

    for (auto &cur_row : list_range<adj_t>(fin)) {
      EXPECT_EQ(cur_row.source, row.source);
      for (size_t i = 0; i < cur_row.targets.size(); i++) {
        EXPECT_EQ(cur_row.targets[i], row.targets[i]);
      }
    }
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
