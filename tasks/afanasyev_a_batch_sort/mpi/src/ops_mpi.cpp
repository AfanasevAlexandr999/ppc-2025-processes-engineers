#include "afanasyev_a_batch_sort/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cstdlib>
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
  int rank = 0, size = 1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int n = GetInput();
  std::vector<InType> data;
  if (rank == 0) {
    data.resize(n);
    for (int i = 0; i < n; i++) {
      data[i] = rand() % 1000;
    }
  }

  int base_local_size = n / size;
  int remainder = n % size;
  int local_size = base_local_size + (rank < remainder ? 1 : 0);
  std::vector<InType> local_data(local_size);

  std::vector<int> counts(size);
  std::vector<int> displs(size);
  for (int i = 0; i < size; i++) {
    counts[i] = base_local_size + (i < remainder ? 1 : 0);
    displs[i] = (i == 0) ? 0 : displs[i - 1] + counts[i - 1];
  }

  MPI_Scatterv(data.data(), counts.data(), displs.data(), MPI_INT, local_data.data(), local_size, MPI_INT, 0,
               MPI_COMM_WORLD);

  RadixSort(local_data);

  int step = 1;
  while (step < size) {
    if ((rank / step) % 2 == 0) {
      int partner = rank + step;
      if (partner < size) {
        int recv_size;
        MPI_Recv(&recv_size, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::vector<InType> recv_data(recv_size);
        MPI_Recv(recv_data.data(), recv_size, MPI_INT, partner, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        local_data = BatcherMerge(local_data, recv_data);
      }
    } else {
      int partner = rank - step;
      int send_size = local_data.size();
      MPI_Send(&send_size, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
      MPI_Send(local_data.data(), send_size, MPI_INT, partner, 1, MPI_COMM_WORLD);
      local_data.clear();
    }
    step *= 2;
  }

  int final_size = (rank == 0) ? static_cast<int>(local_data.size()) : 0;
  MPI_Bcast(&final_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
  GetOutput() = final_size;

  MPI_Barrier(MPI_COMM_WORLD);
  return true;
}

bool AfanasyevABatchSortMPI::PostProcessingImpl() {
  return GetOutput() > 0;
}

}  // namespace afanasyev_a_batch_sort
