#include <gtest/gtest.h>

#include "afanasyev_a_batch_sort/common/include/common.hpp"
#include "afanasyev_a_batch_sort/mpi/include/ops_mpi.hpp"
#include "afanasyev_a_batch_sort/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace afanasyev_a_batch_sort {

class AfanasyevABatchSortPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
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

TEST_P(AfanasyevABatchSortPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, AfanasyevABatchSortMPI, AfanasyevABatchSortSEQ>(PPC_SETTINGS_afanasyev_a_batch_sort);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = AfanasyevABatchSortPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, AfanasyevABatchSortPerfTests, kGtestValues, kPerfTestName);

}  // namespace afanasyev_a_batch_sort
