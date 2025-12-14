#include "afanasyev_a_it_seidel_method/seq/include/ops_seq.hpp"

#include <cmath>
#include <stdexcept>
#include <vector>

#include "afanasyev_a_it_seidel_method/common/include/common.hpp"
#include "util/include/util.hpp"

namespace afanasyev_a_it_seidel_method {

AfanasyevAItSeidelMethodSEQ::AfanasyevAItSeidelMethodSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = std::vector<double>();
}

bool AfanasyevAItSeidelMethodSEQ::ValidationImpl() {
  const auto &input = GetInput();
  if (input.size() < 3) {
    return false;
  }

  int system_size = static_cast<int>(input[0]);
  double epsilon = input[1];
  int max_iterations = static_cast<int>(input[2]);

  if (system_size <= 0) {
    return false;
  }

  if (epsilon <= 0) {
    return false;
  }

  if (max_iterations <= 0) {
    return false;
  }

  return true;
}

bool AfanasyevAItSeidelMethodSEQ::PreProcessingImpl() {
  try {
    int system_size = static_cast<int>(GetInput()[0]);
    epsilon_ = GetInput()[1];
    max_iterations_ = static_cast<int>(GetInput()[2]);

    A_.resize(system_size, std::vector<double>(system_size, 0.0));
    for (int i = 0; i < system_size; ++i) {
      for (int j = 0; j < system_size; ++j) {
        if (i == j) {
          A_[i][j] = system_size + 1.0;
        } else {
          A_[i][j] = 1.0 / (std::abs(i - j) + 1.0);
        }
      }
    }

    b_.resize(system_size, 0.0);
    for (int i = 0; i < system_size; ++i) {
      b_[i] = i + 1.0;
    }

    x_.resize(system_size, 0.0);

    return true;
  } catch (...) {
    return false;
  }
}

bool AfanasyevAItSeidelMethodSEQ::RunImpl() {
  try {
    int system_size = static_cast<int>(A_.size());
    if (system_size == 0) {
      return false;
    }

    std::vector<double> prev_x(system_size, 0.0);

    for (int iter = 0; iter < max_iterations_; ++iter) {
      // Безопасное копирование с проверкой
      if (prev_x.size() == x_.size()) {
        for (int i = 0; i < system_size; ++i) {
          prev_x[i] = x_[i];
        }
      }

      for (int i = 0; i < system_size; ++i) {
        double sum = b_[i];

        for (int j = 0; j < i; ++j) {
          sum -= A_[i][j] * x_[j];
        }

        for (int j = i + 1; j < system_size; ++j) {
          sum -= A_[i][j] * x_[j];
        }

        x_[i] = sum / A_[i][i];
      }

      double max_diff = 0.0;
      for (int i = 0; i < system_size; ++i) {
        double diff = std::abs(x_[i] - prev_x[i]);
        if (diff > max_diff) {
          max_diff = diff;
        }
      }

      if (max_diff < epsilon_) {
        break;
      }
    }

    GetOutput() = x_;

    return true;
  } catch (...) {
    return false;
  }
}

bool AfanasyevAItSeidelMethodSEQ::PostProcessingImpl() {
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
