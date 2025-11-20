#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <random>
#include <vector>

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
    if (kVectorSize <= 0) {
      input_data_ = {};
      expected_output_ = 0.0;
    } else {
      input_data_.resize(kVectorSize);

      // NOLINT используется, чтобы заглушить ошибку clang-tidy о фиксированном сиде.
      std::mt19937 gen(42);  // NOLINT(cert-msc51-cpp)
      std::uniform_int_distribution<> distrib(-1000, 1000);

      for (int i = 0; i < kVectorSize; ++i) {
        input_data_[i] = distrib(gen);
      }

      // Fix: используем long long
      long long sum = std::accumulate(input_data_.begin(), input_data_.end(), 0LL);
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
  OutType expected_output_ = 0.0;  // Fix: Инициализация члена класса
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
