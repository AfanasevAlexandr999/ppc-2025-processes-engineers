#include <gtest/gtest.h>

#include "afanasyev_a_it_seidel_method/common/include/common.hpp"
#include "afanasyev_a_it_seidel_method/mpi/include/ops_mpi.hpp"
#include "afanasyev_a_it_seidel_method/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace afanasyev_a_it_seidel_method {

class ExampleRunPerfTestProcesses2 : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const int kCount_ = 100;
  InType input_data_{};

  void SetUp() override {
    input_data_ = kCount_;
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return input_data_ == output_data;
  }

  InType GetTestInputData() final {
    return input_data_;
  }
};

TEST_P(ExampleRunPerfTestProcesses2, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, AfanasyevAItSeidelMethodMPI, AfanasyevAItSeidelMethodSEQ>(PPC_SETTINGS_afanasyev_a_it_seidel_method);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = ExampleRunPerfTestProcesses2::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, ExampleRunPerfTestProcesses2, kGtestValues, kPerfTestName);

}  // namespace afanasyev_a_it_seidel_method
