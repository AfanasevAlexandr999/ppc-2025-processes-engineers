#include "afanasyev_a_it_seidel_method/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <cmath>
#include <stdexcept>
#include <vector>

#include "afanasyev_a_it_seidel_method/common/include/common.hpp"
#include "util/include/util.hpp"

namespace afanasyev_a_it_seidel_method {

AfanasyevAItSeidelMethodMPI::AfanasyevAItSeidelMethodMPI(const InType &in) {
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
    double epsilon = GetInput()[1];
    int max_iterations = static_cast<int>(GetInput()[2]);

    A_.clear();
    A_.reserve(system_size);
    for (int i = 0; i < system_size; ++i) {
      std::vector<double> row;
      row.reserve(system_size);
      for (int j = 0; j < system_size; ++j) {
        if (i == j) {
          row.push_back(system_size + 1.0);
        } else {
          row.push_back(1.0 / (std::abs(i - j) + 1.0));
        }
      }
      A_.push_back(std::move(row));
    }

    b_.clear();
    b_.reserve(system_size);
    for (int i = 0; i < system_size; ++i) {
      b_.push_back(i + 1.0);
    }

    x_.clear();
    x_.resize(system_size, 0.0);

    epsilon_ = epsilon;
    max_iterations_ = max_iterations;

    return true;
  } catch (...) {
    return false;
  }
}

bool AfanasyevAItSeidelMethodMPI::RunImpl() {
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int system_size = static_cast<int>(A_.size());
  if (system_size == 0) {
    return false;
  }

  int rows_per_process = system_size / size;
  int remainder = system_size % size;

  int start_row = rank * rows_per_process + std::min(rank, remainder);
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
        if (diff > max_diff) {
          max_diff = diff;
        }
      }

      if (max_diff < epsilon_) {
        int converged = 1;
        MPI_Bcast(&converged, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (x_.size() != global_x.size()) {
          x_.resize(global_x.size());
        }
        for (size_t i = 0; i < global_x.size(); ++i) {
          x_[i] = global_x[i];
        }
        break;
      }

      int converged = 0;
      MPI_Bcast(&converged, 1, MPI_INT, 0, MPI_COMM_WORLD);
    } else {
      int converged;
      MPI_Bcast(&converged, 1, MPI_INT, 0, MPI_COMM_WORLD);
      if (converged) {
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
    for (size_t i = 0; i < x_.size(); ++i) {
      output.push_back(x_[i]);
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
    if (system_size == 0 || x_.size() != static_cast<size_t>(system_size)) {
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
