#include "c_api.h"
#include "c_api_error.h"
#include "csr_matrix.hpp"
#include "tools.hpp"

#include <iostream>
#include <memory>

GSC_DLL auto CSRMatrixLoadFromFile(const char *fname, CSRMatrixHandle *out)
    -> int {
  API_BEGIN();
  *out = new std::shared_ptr<CSR>(CSR::load(fname));
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
  CSRMatrixHandle handle = args->csr_handle;
  API_BEGIN();
  CHECK_HANDLE();
  CSR *m = static_cast<std::shared_ptr<CSR> *>(handle)->get();
  auto dptr = new std::shared_ptr<Dense>(
      m->slice(args->idxset, static_cast<std::size_t>(args->len)));
  args->handle_out = dptr;
  auto dptr_inner = dptr->get();
  args->data_out = dptr_inner->data.data();
  args->nrows_out = dptr_inner->nrows;
  args->ncols_out = dptr_inner->ncols;
  API_END();
}

GSC_DLL auto CSRMatrixFree(CSRMatrixHandle handle) -> int {
  API_BEGIN();
  CHECK_HANDLE();
  delete static_cast<std::shared_ptr<CSR> *>(handle);
  API_END();
}

GSC_DLL auto DenseMatrixFree(DenseMatrixHandle handle) -> int {
  API_BEGIN();
  CHECK_HANDLE();
  delete static_cast<std::shared_ptr<Dense> *>(handle);
  API_END();
}
