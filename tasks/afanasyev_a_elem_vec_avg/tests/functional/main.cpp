#include <gtest/gtest.h>

#include <array>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <random>
#include <string>
#include <tuple>

#include "afanasyev_a_elem_vec_avg/common/include/common.hpp"
#include "afanasyev_a_elem_vec_avg/mpi/include/ops_mpi.hpp"
#include "afanasyev_a_elem_vec_avg/seq/include/ops_seq.hpp"
#include "util/include/func_test_util.hpp"
#include "util/include/util.hpp"

namespace afanasyev_a_elem_vec_avg {

// Определяем тип параметров теста: <Размер вектора, Название теста>

class AfanasyevAElemVecAvgFuncTests : public ppc::util::BaseRunFuncTests<InType, OutType, TestType> {
 public:
  static std::string PrintTestParam(const TestType &test_param) {
    // Формируем уникальное имя для каждого теста
    return "Size_" + std::to_string(std::get<0>(test_param)) + "_" + std::get<1>(test_param);
  }

 protected:
  void SetUp() override {
    // Получаем параметры текущего теста
    TestType params = std::get<static_cast<std::size_t>(ppc::util::GTestParamIndex::kTestParams)>(GetParam());
    int vector_size = std::get<0>(params);

    // 1. Генерация тестовых данных
    if (vector_size <= 0) {
      input_data_ = {};
      expected_output_ = 0.0;
    } else {
      input_data_.resize(vector_size);

      // Используем генератор случайных чисел
      std::mt19937 gen(42);
      std::uniform_int_distribution<> distrib(-1000, 1000);

      for (int i = 0; i < vector_size; ++i) {
        input_data_[i] = distrib(gen);
      }

      // 2. Вычисление ожидаемого результата (Reference result)
      // Используем long long для суммы во избежание переполнения
      int64_t sum = std::accumulate(input_data_.begin(), input_data_.end(), 0LL);
      expected_output_ = static_cast<double>(sum) / vector_size;
    }
  }

  // Сравнение результата с учетом погрешности floating-point
  bool CheckTestOutputData(OutType &output_data) final {
    const double tolerance = 1e-5;
    return std::abs(output_data - expected_output_) < tolerance;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

 private:
  InType input_data_;
  OutType expected_output_;
};

namespace {

TEST_P(AfanasyevAElemVecAvgFuncTests, CalculateAverage) {
  ExecuteTest(GetParam());
}

// Параметры тестов: {Размер вектора, Уникальное имя суффикса}
// Имена должны быть уникальными, чтобы избежать ошибки "Duplicate parameterized test name"
const std::array<TestType, 5> kTestParam = {std::make_tuple(100, "Normal"), std::make_tuple(10, "Small"),
                                            std::make_tuple(0, "EmptyVector"),  // Уникальное имя для размера 0
                                            std::make_tuple(1, "SingleElement"), std::make_tuple(10000, "Large")};

// Регистрация задач (MPI и SEQ)
const auto kTestTasksList = std::tuple_cat(
    ppc::util::AddFuncTask<AfanasyevAElemVecAvgMPI, InType>(kTestParam, PPC_SETTINGS_afanasyev_a_elem_vec_avg),
    ppc::util::AddFuncTask<AfanasyevAElemVecAvgSEQ, InType>(kTestParam, PPC_SETTINGS_afanasyev_a_elem_vec_avg));

const auto kGtestValues = ppc::util::ExpandToValues(kTestTasksList);

const auto kPerfTestName = AfanasyevAElemVecAvgFuncTests::PrintFuncTestName<AfanasyevAElemVecAvgFuncTests>;

INSTANTIATE_TEST_SUITE_P(VectorAverageTests, AfanasyevAElemVecAvgFuncTests, kGtestValues, kPerfTestName);

}  // namespace

}  // namespace afanasyev_a_elem_vec_avg
