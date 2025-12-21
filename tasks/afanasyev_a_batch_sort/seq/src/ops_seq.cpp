#include "afanasyev_a_batch_sort/seq/include/ops_seq.hpp"

#include <algorithm>
#include <numeric>
#include <vector>

#include "afanasyev_a_batch_sort/common/include/common.hpp"
#include "util/include/util.hpp"

namespace afanasyev_a_batch_sort {

static void RadixSort(std::vector<InType> &data) {
  if (data.empty()) {
    return;
  }

  InType max_val = *std::max_element(data.begin(), data.end());
  for (InType exp = 1; max_val / exp > 0; exp *= 10) {
    std::vector<InType> output(data.size());
    int count[10] = {0};

    for (auto num : data) {
      count[(num / exp) % 10]++;
    }
    for (int i = 1; i < 10; i++) {
      count[i] += count[i - 1];
    }
    for (int i = data.size() - 1; i >= 0; i--) {
      int digit = (data[i] / exp) % 10;
      output[count[digit] - 1] = data[i];
      count[digit]--;
    }
    data = output;
  }
}

static std::vector<InType> BatcherMerge(const std::vector<InType> &a, const std::vector<InType> &b) {
  std::vector<InType> merged(a.size() + b.size());
  std::merge(a.begin(), a.end(), b.begin(), b.end(), merged.begin());

  bool sorted = false;
  while (!sorted) {
    sorted = true;
    for (size_t i = 0; i + 1 < merged.size(); i += 2) {
      if (merged[i] > merged[i + 1]) {
        std::swap(merged[i], merged[i + 1]);
        sorted = false;
      }
    }
    for (size_t i = 1; i + 1 < merged.size(); i += 2) {
      if (merged[i] > merged[i + 1]) {
        std::swap(merged[i], merged[i + 1]);
        sorted = false;
      }
    }
  }
  return merged;
}

AfanasyevABatchSortSEQ::AfanasyevABatchSortSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool AfanasyevABatchSortSEQ::ValidationImpl() {
  return GetInput() > 0;
}

bool AfanasyevABatchSortSEQ::PreProcessingImpl() {
  return true;
}

bool AfanasyevABatchSortSEQ::RunImpl() {
  int n = GetInput();
  std::vector<InType> data(n);
  for (int i = 0; i < n; i++) {
    data[i] = rand() % 1000;
  }

  RadixSort(data);

  std::vector<InType> sorted = BatcherMerge(data, std::vector<InType>{});

  GetOutput() = static_cast<OutType>(sorted.size());
  return true;
}

bool AfanasyevABatchSortSEQ::PostProcessingImpl() {
  return GetOutput() > 0;
}

}  // namespace afanasyev_a_batch_sort
