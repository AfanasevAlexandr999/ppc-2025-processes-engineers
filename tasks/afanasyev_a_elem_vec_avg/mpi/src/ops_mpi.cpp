#include "afanasyev_a_elem_vec_avg/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <numeric>
#include <vector>

#include "afanasyev_a_elem_vec_avg/common/include/common.hpp"
#include "util/include/util.hpp"

namespace afanasyev_a_elem_vec_avg {

AfanasyevAElemVecAvgMPI::AfanasyevAElemVecAvgMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  // Используем 0.0, так как OutType должен быть double
  GetOutput() = 0.0;
}

bool AfanasyevAElemVecAvgMPI::ValidationImpl() {
  // Мы разрешаем пустые векторы, так как RunImpl умеет их обрабатывать (возвращает 0.0)
  return true;
}

bool AfanasyevAElemVecAvgMPI::PreProcessingImpl() {
  return true;
}

bool AfanasyevAElemVecAvgMPI::RunImpl() {
  int rank, num_processes;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

  const InType &global_vec = GetInput();
  int global_n = global_vec.size();

  if (global_n == 0) {
    GetOutput() = 0.0;
    // Результат должен быть распределен, чтобы все ранги прошли проверку для пустого вектора
    MPI_Bcast(static_cast<void *>(&GetOutput()), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    return true;
  }

  // 1. Расчет массивов send_counts и displs
  std::vector<int> send_counts(num_processes);
  std::vector<int> displs(num_processes);
  int chunk_size = global_n / num_processes;
  int remainder = global_n % num_processes;
  int current_displacement = 0;

  for (int i = 0; i < num_processes; ++i) {
    int count = chunk_size + (i < remainder ? 1 : 0);
    send_counts[i] = count;
    displs[i] = current_displacement;
    current_displacement += count;
  }

  // Получаем размер локального блока и буфер
  int local_n = send_counts[rank];
  std::vector<T> local_vec(local_n);

  // 2. Распределение данных с помощью MPI_Scatterv
  MPI_Scatterv(rank == 0 ? global_vec.data() : nullptr, send_counts.data(), displs.data(), MPI_INT, local_vec.data(),
               local_n, MPI_INT, 0, MPI_COMM_WORLD);

  // 3. Локальное вычисление суммы (используем long long)
  long long local_sum = std::accumulate(local_vec.begin(), local_vec.end(), 0LL);

  // 4. Глобальное суммирование (MPI_Reduce)
  long long global_sum = 0;
  MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

  // 5. Расчет среднего значения (только процесс 0)
  if (rank == 0) {
    GetOutput() = static_cast<OutType>(global_sum) / global_n;
  }

  // 6. РАСПРОСТРАНЕНИЕ РЕЗУЛЬТАТА: КЛЮЧЕВОЙ МОМЕНТ для GTest
  MPI_Bcast(static_cast<void *>(&GetOutput()), 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  return true;
}

bool AfanasyevAElemVecAvgMPI::PostProcessingImpl() {
  return true;
}

}  // namespace afanasyev_a_elem_vec_avg
