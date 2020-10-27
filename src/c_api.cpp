#include "c_api.h"
#include "c_api_error.h"
#include "csr_matrix.hpp"
#include "tools.hpp"

#include <iostream>
#include <memory>

GSC_DLL auto CSRMatrixLoadFromFile(LoadArgs *args) -> int {
  API_BEGIN();
  auto handle = new std::shared_ptr<CSR>(CSR::load(args->fname));
  args->handle = handle;
  auto m = handle->get();
  args->nrows_out = m->_nrows;
  args->ncols_out = m->_ncols;
  API_END();
}

GSC_DLL auto CSRMatrixSaveBinary(CSRMatrixHandle handle, const char *fname)
    -> int {
  API_BEGIN();
  CHECK_HANDLE();
  auto m = static_cast<std::shared_ptr<CSR> *>(handle)->get();
  m->save(fname);
  API_END();
}

GSC_DLL auto DenseMatrixSliceCSRMatrix(SliceArgs *args) -> int {
  CSRMatrixHandle handle = args->handle;
  API_BEGIN();
  CHECK_HANDLE();
  CSR *m = static_cast<std::shared_ptr<CSR> *>(handle)->get();
  auto dptr = m->slice(args->idxset, static_cast<std::size_t>(args->len));
  args->data_out = dptr;
  API_END();
}

GSC_DLL auto CSRMatrixFree(CSRMatrixHandle handle) -> int {
  API_BEGIN();
  CHECK_HANDLE();
  delete static_cast<std::shared_ptr<CSR> *>(handle);
  API_END();
}
