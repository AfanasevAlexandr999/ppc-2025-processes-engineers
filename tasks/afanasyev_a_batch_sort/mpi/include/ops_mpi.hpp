#pragma once

#include "afanasyev_a_batch_sort/common/include/common.hpp"
#include "task/include/task.hpp"

namespace afanasyev_a_batch_sort {

class AfanasyevABatchSortMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit AfanasyevABatchSortMPI(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace afanasyev_a_batch_sort
