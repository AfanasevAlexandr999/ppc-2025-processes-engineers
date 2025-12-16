#include "afanasyev_a_it_seidel_method/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

#include "afanasyev_a_it_seidel_method/common/include/common.hpp"

namespace afanasyev_a_it_seidel_method {

AfanasyevAItSeidelMethodMPI::AfanasyevAItSeidelMethodMPI(const InType &in) : epsilon_(0.0), max_iterations_(0) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::vector<double>();
}

bool AfanasyevAItSeidelMethodMPI::ValidationImpl() {
  return GetInput().size() >= 3;
}

bool AfanasyevAItSeidelMethodMPI::PreProcessingImpl() {
  try {
    int system_size = static_cast<int>(GetInput()[0]);
    epsilon_ = GetInput()[1];
    max_iterations_ = static_cast<int>(GetInput()[2]);

    A_.clear();
    A_.resize(system_size);
    for (int i = 0; i < system_size; ++i) {
      A_[i].resize(system_size);
      for (int j = 0; j < system_size; ++j) {
        if (i == j) {
          A_[i][j] = system_size + 1.0;
        } else {
          A_[i][j] = 1.0 / (std::abs(i - j) + 1.0);
        }
      }
    }

    b_.clear();
    b_.resize(system_size);
    for (int i = 0; i < system_size; ++i) {
      b_[i] = i + 1.0;
    }

    x_.clear();
    x_.resize(system_size, 0.0);

    return true;
  } catch (...) {
    return false;
  }
}

bool AfanasyevAItSeidelMethodMPI::RunImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int system_size = static_cast<int>(A_.size());
  if (system_size == 0) {
    return false;
  }

  int rows_per_process = system_size / size;
  int remainder = system_size % size;

  int start_row = (rank * rows_per_process) + std::min(rank, remainder);
  int end_row = start_row + rows_per_process + (rank < remainder ? 1 : 0);

  std::vector<double> local_x(system_size, 0.0);
  std::vector<double> global_x(system_size, 0.0);

  for (int iter = 0; iter < max_iterations_; ++iter) {
    std::vector<double> prev_x(system_size);
    for (int i = 0; i < system_size; ++i) {
      prev_x[i] = global_x[i];
    }

    for (int i = start_row; i < end_row; ++i) {
      if (i >= system_size) {
        break;
      }

      double sum = b_[i];

      for (int j = 0; j < i; ++j) {
        sum -= A_[i][j] * global_x[j];
      }

      for (int j = i + 1; j < system_size; ++j) {
        sum -= A_[i][j] * global_x[j];
      }

      local_x[i] = sum / A_[i][i];
    }

    MPI_Allgather(local_x.data() + start_row, end_row - start_row, MPI_DOUBLE, global_x.data(), end_row - start_row,
                  MPI_DOUBLE, MPI_COMM_WORLD);

    if (rank == 0) {
      double max_diff = 0.0;
      for (int i = 0; i < system_size; ++i) {
        double diff = std::abs(global_x[i] - prev_x[i]);
        max_diff = std::max(diff, max_diff);
      }

      if (max_diff < epsilon_) {
        int converged = 1;
        MPI_Bcast(&converged, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (x_.size() == global_x.size()) {
          for (std::size_t i = 0; i < global_x.size(); ++i) {
            x_[i] = global_x[i];
          }
        }
        break;
      }

      int converged = 0;
      MPI_Bcast(&converged, 1, MPI_INT, 0, MPI_COMM_WORLD);
    } else {
      int converged = 0;
      MPI_Bcast(&converged, 1, MPI_INT, 0, MPI_COMM_WORLD);
      if (converged != 0) {
        MPI_Allgather(local_x.data() + start_row, end_row - start_row, MPI_DOUBLE, global_x.data(), end_row - start_row,
                      MPI_DOUBLE, MPI_COMM_WORLD);
        break;
      }
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);

  if (rank == 0) {
    OutType output;
    output.reserve(x_.size());
    for (const double val : x_) {
      output.push_back(val);
    }
    GetOutput() = output;
  } else {
    GetOutput() = std::vector<double>();
  }

  return true;
}

bool AfanasyevAItSeidelMethodMPI::PostProcessingImpl() {
  try {
    int system_size = static_cast<int>(A_.size());
    if (system_size != static_cast<int>(x_.size())) {
      return false;
    }

    double residual_norm = 0.0;
    for (int i = 0; i < system_size; ++i) {
      double sum = 0.0;
      for (int j = 0; j < system_size; ++j) {
        sum += A_[i][j] * x_[j];
      }
      residual_norm += std::abs(sum - b_[i]);
    }

    residual_norm /= system_size;
    return residual_norm < epsilon_ * 10;
  } catch (...) {
    return false;
  }
}

}  // namespace afanasyev_a_it_seidel_method
