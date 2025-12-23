#include "afanasyev_a_batch_sort/mpi/include/ops_mpi.hpp"

#include <mpi.h>

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
    for (const InType num : data) {
      const auto digit = static_cast<std::size_t>((num / exp) % 10);
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

std::vector<InType> BatcherMerge(const std::vector<InType> &a, const std::vector<InType> &b) {
  std::vector<InType> merged(a.size() + b.size());
  std::ranges::merge(a, b, merged.begin());
  return merged;
}

std::vector<InType> GenerateData(std::size_t n) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<InType> dist(0, 1000);

  std::vector<InType> data(n);
  for (auto &v : data) {
    v = dist(gen);
  }
  return data;
}

}  // namespace

AfanasyevABatchSortMPI::AfanasyevABatchSortMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool AfanasyevABatchSortMPI::ValidationImpl() {
  return GetInput() > 0;
}

bool AfanasyevABatchSortMPI::PreProcessingImpl() {
  return true;
}

bool AfanasyevABatchSortMPI::RunImpl() {
  int rank = 0;
  int size = 1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  const auto n = static_cast<std::size_t>(GetInput());

  std::vector<InType> global_data;
  if (rank == 0) {
    global_data = GenerateData(n);
  }

  const std::size_t base = n / static_cast<std::size_t>(size);
  const std::size_t rem = n % static_cast<std::size_t>(size);

  // Используем std::cmp_less для безопасного сравнения знаковых и беззнаковых
  const std::size_t local_size = base + (std::cmp_less(rank, static_cast<int>(rem)) ? 1U : 0U);

  std::vector<int> counts(size);
  std::vector<int> displs(size);

  std::size_t offset = 0;
  for (int i = 0; i < size; ++i) {
    // Используем std::cmp_less для безопасного сравнения
    const std::size_t sz = base + (std::cmp_less(i, static_cast<int>(rem)) ? 1U : 0U);
    counts.at(i) = static_cast<int>(sz);
    displs.at(i) = static_cast<int>(offset);
    offset += sz;
  }

  std::vector<InType> local_data(local_size);

  MPI_Datatype mpi_in_type = MPI_INT;

  MPI_Scatterv(global_data.data(), counts.data(), displs.data(), mpi_in_type, local_data.data(),
               static_cast<int>(local_size), mpi_in_type, 0, MPI_COMM_WORLD);

  RadixSort(local_data);

  int step = 1;
  while (step < size) {
    if ((rank / step) % 2 == 0) {
      const int partner = rank + step;
      if (partner < size) {
        int recv_size = 0;
        MPI_Recv(&recv_size, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        std::vector<InType> recv_data(static_cast<std::size_t>(recv_size));

        MPI_Recv(recv_data.data(), recv_size, mpi_in_type, partner, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        local_data = BatcherMerge(local_data, recv_data);
      }
    } else {
      const int partner = rank - step;
      const int send_size = static_cast<int>(local_data.size());

      MPI_Send(&send_size, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
      MPI_Send(local_data.data(), send_size, mpi_in_type, partner, 1, MPI_COMM_WORLD);
      break;
    }
    step *= 2;
  }

  int result = 0;
  if (rank == 0) {
    result = static_cast<int>(local_data.size());
  }

  MPI_Bcast(&result, 1, MPI_INT, 0, MPI_COMM_WORLD);
  GetOutput() = result;

  return true;
}

bool AfanasyevABatchSortMPI::PostProcessingImpl() {
  return GetOutput() > 0;
}

}  // namespace afanasyev_a_batch_sort
