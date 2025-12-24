#include <gtest/gtest.h>

#include <array>
#include <cstddef>
#include <string>
#include <tuple>

#include "afanasyev_a_batch_sort/common/include/common.hpp"
#include "afanasyev_a_batch_sort/mpi/include/ops_mpi.hpp"
#include "afanasyev_a_batch_sort/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace afanasyev_a_batch_sort {

class AfanasyevABatchSortFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    return std::to_string(std::get<0>(test_param)) + "_" + std::get<1>(test_param);
  }

 protected:
  void SetUp() override {
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    input_data_ = std::get<0>(params);
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return output_data == input_data_;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_ = 0;
};

namespace {

TEST_P(AfanasyevABatchSortFuncTests, RadixBatcherSortTest) {
  ExecuteTest(GetParam());
}

const std::array<TestType, 6> kTestParam = {std::make_tuple(1, "1"),   std::make_tuple(3, "3"),
                                            std::make_tuple(5, "5"),   std::make_tuple(7, "7"),
                                            std::make_tuple(10, "10"), std::make_tuple(100, "100")};

const auto kTestTasksList = std::tuple_cat(
    ppc::util::AddFuncTask<AfanasyevABatchSortMPI, InType>(kTestParam, PPC_SETTINGS_afanasyev_a_batch_sort),
    ppc::util::AddFuncTask<AfanasyevABatchSortSEQ, InType>(kTestParam, PPC_SETTINGS_afanasyev_a_batch_sort));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = AfanasyevABatchSortFuncTests::PrintFuncTestName<AfanasyevABatchSortFuncTests>;

INSTANTIATE_TEST_SUITE_P(RadixBatcherTests, AfanasyevABatchSortFuncTests, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace afanasyev_a_batch_sort
