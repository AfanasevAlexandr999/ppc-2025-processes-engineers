#include "afanasyev_a_elem_vec_avg/seq/include/ops_seq.hpp"

#include <numeric>
#include <vector>
#include <cstdint> // Fix: header for int64_t if needed

#include "afanasyev_a_elem_vec_avg/common/include/common.hpp"
#include "util/include/util.hpp"

namespace afanasyev_a_elem_vec_avg {

using InType = std::vector<int>; 
using OutType = double;

AfanasyevAElemVecAvgSEQ::AfanasyevAElemVecAvgSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0.0; 
}

bool AfanasyevAElemVecAvgSEQ::ValidationImpl() {
  return true;
}

bool AfanasyevAElemVecAvgSEQ::PreProcessingImpl() {
  return true;
}

bool AfanasyevAElemVecAvgSEQ::RunImpl() {
  const InType& vec = GetInput();
  int size = static_cast<int>(vec.size());

  if (size == 0) {
    GetOutput() = 0.0;
    return true; 
  }

  // Используем long long
  long long sum = std::accumulate(vec.begin(), vec.end(), 0LL);
  
  // Fix: narrowing conversion warning
  GetOutput() = static_cast<OutType>(sum) / static_cast<double>(size);

  return true; 
}

bool AfanasyevAElemVecAvgSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace afanasyev_a_elem_vec_avg