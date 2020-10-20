// "Copyright 2020 Kirill Konevets"

/*! \mainpage gscounting (GraphSage Counting)
 *
 * \section intro Introduction
 *
 * Multiple label propagation on graphs with GraphSage. Classical algorithms,
 * like Label Propagation, solve this task in a deterministic way. This is a
 * probabilistic aproach using GraphSage algorithm with neighbor
 * label counts as node features.
 *
 */

#include <algorithm>
#include <cassert>
#include <chrono>
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

int main() { return 0; }
