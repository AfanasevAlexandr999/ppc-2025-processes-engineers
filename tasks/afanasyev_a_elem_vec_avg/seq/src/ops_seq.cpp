#include "afanasyev_a_elem_vec_avg/seq/include/ops_seq.hpp"

#include <numeric>
#include <vector>

#include "afanasyev_a_elem_vec_avg/common/include/common.hpp"

namespace afanasyev_a_elem_vec_avg {

AfanasyevAElemVecAvgSEQ::AfanasyevAElemVecAvgSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  // Используем 0.0, так как OutType должен быть double
  GetOutput() = 0.0;
}

bool AfanasyevAElemVecAvgSEQ::ValidationImpl() {
  // Аналогично разрешаем пустые векторы
  return true;
}

bool AfanasyevAElemVecAvgSEQ::PreProcessingImpl() {
  return true;
}

bool AfanasyevAElemVecAvgSEQ::RunImpl() {
  const InType &vec = GetInput();
  int size = vec.size();

  if (size == 0) {
    GetOutput() = 0.0;
    return true;
  }

  // 1. Вычисление суммы всех элементов. Используем long long для суммы,
  // чтобы избежать переполнения.
  long long sum = std::accumulate(vec.begin(), vec.end(), 0LL);

  // 2. Вычисление среднего значения.
  GetOutput() = static_cast<OutType>(sum) / size;

  return true;
}

bool AfanasyevAElemVecAvgSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace afanasyev_a_elem_vec_avg
