#pragma once

#include <vector>

#include "afanasyev_a_it_seidel_method/common/include/common.hpp"

namespace afanasyev_a_it_seidel_method {

class AfanasyevAItSeidelMethodSEQ : public BaseTask {
 public:
  explicit AfanasyevAItSeidelMethodSEQ(const InType &in);
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

 private:
  std::vector<std::vector<double>> A_;
  std::vector<double> b_;
  std::vector<double> x_;
  double epsilon_;
  int max_iterations_;
};

}  // namespace afanasyev_a_it_seidel_method
