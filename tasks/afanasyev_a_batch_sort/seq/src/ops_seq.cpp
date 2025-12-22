#include "afanasyev_a_batch_sort/seq/include/ops_seq.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <random>
#include <utility>
#include <vector>

#include "afanasyev_a_batch_sort/common/include/common.hpp"

namespace afanasyev_a_batch_sort {
namespace {

void RadixSort(std::vector<InType> &data) {
  if (data.empty()) {
    return;
  }

  const InType max_val = *std::ranges::max_element(data);
  std::vector<InType> output(data.size());

  for (InType exp = 1; max_val / exp > 0; exp *= 10) {
    std::array<std::size_t, 10> count{};
    count.fill(0);

    // Подсчет цифр
    for (const InType value : data) {
      const std::size_t digit = static_cast<std::size_t>((value / exp) % 10);
      // Используем прямую индексацию, так как digit гарантированно 0-9
      if (digit < 10) {
        count[digit]++;
      }
    }

    // Префиксная сумма
    for (std::size_t i = 1; i < 10; ++i) {
      count[i] += count[i - 1];
    }

    // Размещение элементов
    for (std::size_t i = data.size(); i-- > 0;) {
      const std::size_t digit = static_cast<std::size_t>((data[i] / exp) % 10);
      if (digit < 10 && count[digit] > 0) {
        output[count[digit] - 1] = data[i];
        count[digit]--;
      }
    }

    // Копируем результат обратно
    std::ranges::copy(output, data.begin());
  }
}

std::vector<InType> GenerateData(std::size_t n) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<InType> dist(0, 1000);

  std::vector<InType> data(n);
  for (auto &value : data) {
    value = dist(gen);
  }
  return data;
}

}  // namespace

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
  const std::size_t n = static_cast<std::size_t>(GetInput());

  std::vector<InType> data = GenerateData(n);
  RadixSort(data);

  GetOutput() = static_cast<OutType>(data.size());

  return true;
}

bool AfanasyevABatchSortSEQ::PostProcessingImpl() {
  return GetOutput() > 0;
}

}  // namespace afanasyev_a_batch_sort
