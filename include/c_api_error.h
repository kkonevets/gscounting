/*!
 * Copyright (c) 2020 by Contributors
 * \file c_api_error.h
 * \author Kirill Konevets
 * \brief Error handling for C API.
 */

#ifndef GSCOUNTING_C_API_ERROR_H_
#define GSCOUNTING_C_API_ERROR_H_

#define API_BEGIN() try {

#define API_END()                                                              \
  }                                                                            \
  catch (std::exception const &_except_) {                                     \
    std::cerr << _except_.what();                                              \
    return -1;                                                                 \
  }                                                                            \
  return 0;

#define CHECK_HANDLE()                                                         \
  if (handle == nullptr)                                                       \
    std::cerr                                                                  \
        << "CSRMatrix/DenseMatrix has not been intialized or has already "     \
           "been disposed.";

#endif // GSCOUNTING_C_API_ERROR_H_
