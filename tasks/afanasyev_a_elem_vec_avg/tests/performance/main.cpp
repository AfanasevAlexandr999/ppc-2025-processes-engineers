#include <gtest/gtest.h>

#include <cmath>
#include <cstdint>
#include <numeric>
#include <random>

#include "afanasyev_a_elem_vec_avg/common/include/common.hpp"
#include "afanasyev_a_elem_vec_avg/mpi/include/ops_mpi.hpp"
#include "afanasyev_a_elem_vec_avg/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace afanasyev_a_elem_vec_avg {

class AfanasyevAElemVecAvgPerfTests : public ppc::util::BaseRunPerfTests<InType, OutType> {
 public:
  static constexpr int kVectorSize = 100000000;

 protected:
  void SetUp() override {
    if constexpr (kVectorSize <= 0) {
      input_data_ = {};
      expected_output_ = 0.0;
    } else {
      input_data_.resize(kVectorSize);

      std::mt19937 gen(42);  // NOLINT
      std::uniform_int_distribution<> distrib(-10, 10);

      for (int i = 0; i < kVectorSize; ++i) {
        input_data_[i] = distrib(gen);
      }

      int64_t sum = std::accumulate(input_data_.begin(), input_data_.end(), static_cast<int64_t>(0));
      expected_output_ = static_cast<double>(sum) / kVectorSize;
    }
  }

  bool CheckTestOutputData(OutType &output_data) final {
    const double tolerance = 1e-5;
    return std::abs(output_data - expected_output_) < tolerance;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
  OutType expected_output_ = 0.0;
};

TEST_P(AfanasyevAElemVecAvgPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks = ppc::util::MakeAllPerfTasks<InType, AfanasyevAElemVecAvgMPI, AfanasyevAElemVecAvgSEQ>(
    PPC_SETTINGS_afanasyev_a_elem_vec_avg);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = AfanasyevAElemVecAvgPerfTests::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(VectorAveragePerfTests, AfanasyevAElemVecAvgPerfTests, kGtestValues, kPerfTestName);

}  // namespace afanasyev_a_elem_vec_avg
