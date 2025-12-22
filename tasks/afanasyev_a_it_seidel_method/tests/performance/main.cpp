#include <gtest/gtest.h>

#include "afanasyev_a_it_seidel_method/common/include/common.hpp"
#include "afanasyev_a_it_seidel_method/mpi/include/ops_mpi.hpp"
#include "afanasyev_a_it_seidel_method/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace afanasyev_a_it_seidel_method {

// Класс тестов производительности
class AfanasyevAItSeidelMethodPerfTests
    : public ppc::util::BaseRunPerfTests<InType, OutType> {
 protected:
  const int kSystemSize = 10;
  const double kEpsilon = 1e-6;
  const int kMaxIterations = 1000;

  InType input_data_;

  void SetUp() override {
    input_data_.clear();
    input_data_.push_back(static_cast<double>(kSystemSize));
    input_data_.push_back(kEpsilon);
    input_data_.push_back(static_cast<double>(kMaxIterations));
  }

  bool CheckTestOutputData(OutType &output_data) override {
    return output_data.size() == static_cast<size_t>(kSystemSize);
  }

  InType GetTestInputData() override {
    return input_data_;
  }

 public:
  // Генерация уникального имени теста для GTest
  static std::string CustomPerfTestName(
      const ::testing::TestParamInfo<ppc::util::PerfTestParam<InType, OutType>> &info) 
  {
      auto base_name = std::get<static_cast<size_t>(ppc::util::GTestParamIndex::kNameTest)>(info.param);
      auto mode = std::get<static_cast<size_t>(ppc::util::GTestParamIndex::kTestParams)>(info.param);

      std::string mode_str = (mode == ppc::performance::PerfResults::TypeOfRunning::kPipeline) 
                             ? "pipeline" : "task";

      return mode_str + "_" + base_name;
  }
};

// Тестовый метод
TEST_P(AfanasyevAItSeidelMethodPerfTests, RunPerfModes) {
  ExecuteTest(GetParam());
}

// Генерация уникальных имен для каждой задачи
template <typename TaskType, typename InputType>
auto MakePerfTaskTuples() {
    std::string name;
    if constexpr (std::is_same_v<TaskType, AfanasyevAItSeidelMethodMPI>) {
        name = "afanasyev_a_it_seidel_method_mpi";
    } else if constexpr (std::is_same_v<TaskType, AfanasyevAItSeidelMethodSEQ>) {
        name = "afanasyev_a_it_seidel_method_seq";
    } else {
        name = "unknown_task_" + std::to_string(rand());
    }

    // Лямбда для отложенного создания задачи с входными данными
    auto task_lambda = [](const InputType &in) {
        return ppc::task::TaskGetter<TaskType, InputType>(in);
    };

    return std::make_tuple(
        std::make_tuple(task_lambda, name,
                        ppc::performance::PerfResults::TypeOfRunning::kPipeline),
        std::make_tuple(task_lambda, name,
                        ppc::performance::PerfResults::TypeOfRunning::kTaskRun)
    );
}

// Создание всех комбинаций задач
const auto kAllPerfTasks = std::tuple_cat(
    MakePerfTaskTuples<AfanasyevAItSeidelMethodMPI, InType>(),
    MakePerfTaskTuples<AfanasyevAItSeidelMethodSEQ, InType>()
);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

// Инстанцирование тестов с уникальными именами
INSTANTIATE_TEST_SUITE_P(RunModeTests,
                         AfanasyevAItSeidelMethodPerfTests,
                         kGtestValues,
                         AfanasyevAItSeidelMethodPerfTests::CustomPerfTestName);

}  // namespace afanasyev_a_it_seidel_method