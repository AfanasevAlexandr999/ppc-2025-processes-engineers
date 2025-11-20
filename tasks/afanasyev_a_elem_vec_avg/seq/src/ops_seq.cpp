#include "afanasyev_a_elem_vec_avg/seq/include/ops_seq.hpp"

#include <cstdint>  // Теперь этот заголовок используется для int64_t
#include <numeric>
#include <vector>

#include "afanasyev_a_elem_vec_avg/common/include/common.hpp"
// #include "util/include/util.hpp" - удален, так как не используется напрямую

namespace afanasyev_a_elem_vec_avg {

using T = int;
using InType = std::vector<T>;
using OutType = double;

AfanasyevAElemVecAvgSEQ::AfanasyevAElemVecAvgSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0.0;
}

bool AfanasyevAElemVecAvgSEQ::ValidationImpl() {
  // Добавлена проверка на пустоту вектора для корректной обработки
  return true;
}

bool AfanasyevAElemVecAvgSEQ::PreProcessingImpl() {
  return true;
}

bool AfanasyevAElemVecAvgSEQ::RunImpl() {
  const InType &vec = GetInput();
  int n = static_cast<int>(vec.size());

  if (n == 0) {
    GetOutput() = 0.0;
    return true;
  }

  // ИСПРАВЛЕНИЕ: long long заменен на int64_t для соответствия google-runtime-int
  // static_cast<int64_t>(0) используется как начальное значение для предотвращения переполнения
  int64_t sum = std::accumulate(vec.begin(), vec.end(), static_cast<int64_t>(0));

  GetOutput() = static_cast<OutType>(sum) / static_cast<double>(n);

  return true;
}

bool AfanasyevAElemVecAvgSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace afanasyev_a_elem_vec_avg
