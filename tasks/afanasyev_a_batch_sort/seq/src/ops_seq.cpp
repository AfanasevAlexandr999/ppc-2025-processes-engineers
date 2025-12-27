#include "afanasyev_a_batch_sort/seq/include/ops_seq.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <random>
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

    // Подсчет цифр с использованием безопасного доступа
    for (const InType value : data) {
      const auto digit = static_cast<std::size_t>((value / exp) % 10);
      if (digit < count.size()) {
        ++count.at(digit);
      }
    }

    // Префиксная сумма с безопасным доступом
    for (std::size_t i = 1; i < count.size(); ++i) {
      count.at(i) += count.at(i - 1);
    }

    // Размещение элементов с безопасным доступом
    for (std::size_t i = data.size(); i-- > 0;) {
      const auto digit = static_cast<std::size_t>((data.at(i) / exp) % 10);
      if (digit < count.size() && count.at(digit) > 0) {
        const std::size_t index = count.at(digit) - 1;
        output.at(index) = data.at(i);
        --count.at(digit);
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
  const auto n = static_cast<std::size_t>(GetInput());

  std::vector<InType> data = GenerateData(n);
  RadixSort(data);

  GetOutput() = static_cast<OutType>(data.size());

  return true;
}

bool AfanasyevABatchSortSEQ::PostProcessingImpl() {
  return GetOutput() > 0;
}

}  // namespace afanasyev_a_batch_sort
