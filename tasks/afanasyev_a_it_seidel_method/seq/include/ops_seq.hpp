#pragma once

#include "afanasyev_a_it_seidel_method/common/include/common.hpp"
#include "task/include/task.hpp"

namespace afanasyev_a_it_seidel_method {

class AfanasyevAItSeidelMethodSEQ : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }
  explicit AfanasyevAItSeidelMethodSEQ(const InType &in);

 private:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace afanasyev_a_it_seidel_method
