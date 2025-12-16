#include <gtest/gtest.h>
#include <mpi.h>

#include <cstddef>

#include "afanasyev_a_it_seidel_method/common/include/common.hpp"
#include "afanasyev_a_it_seidel_method/mpi/include/ops_mpi.hpp"
#include "afanasyev_a_it_seidel_method/seq/include/ops_seq.hpp"

namespace afanasyev_a_it_seidel_method {

// Тесты для последовательной версии
TEST(AfanasyevAItSeidelMethodFuncTests, SeidelMethodSeqSmallSystem) {
  InType input = {3.0, 0.001, 100.0};
  AfanasyevAItSeidelMethodSEQ task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());

  auto output = task.GetOutput();
  EXPECT_EQ(output.size(), static_cast<std::size_t>(3));
}

TEST(AfanasyevAItSeidelMethodFuncTests, SeidelMethodSeqMediumSystem) {
  InType input = {5.0, 0.0001, 200.0};
  AfanasyevAItSeidelMethodSEQ task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());

  auto output = task.GetOutput();
  EXPECT_EQ(output.size(), static_cast<std::size_t>(5));
}

TEST(AfanasyevAItSeidelMethodFuncTests, SeidelMethodSeqLargeSystem) {
  InType input = {7.0, 0.00001, 300.0};
  AfanasyevAItSeidelMethodSEQ task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());

  auto output = task.GetOutput();
  EXPECT_EQ(output.size(), static_cast<std::size_t>(7));
}

// Тесты для MPI версии
TEST(AfanasyevAItSeidelMethodFuncTests, SeidelMethodMpiSmallSystem) {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  InType input = {3.0, 0.001, 100.0};
  AfanasyevAItSeidelMethodMPI task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());

  if (rank == 0) {
    auto output = task.GetOutput();
    EXPECT_EQ(output.size(), static_cast<std::size_t>(3));
  }
}

TEST(AfanasyevAItSeidelMethodFuncTests, SeidelMethodMpiMediumSystem) {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  InType input = {5.0, 0.0001, 200.0};
  AfanasyevAItSeidelMethodMPI task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());

  if (rank == 0) {
    auto output = task.GetOutput();
    EXPECT_EQ(output.size(), static_cast<std::size_t>(5));
  }
}

TEST(AfanasyevAItSeidelMethodFuncTests, SeidelMethodMpiLargeSystem) {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  InType input = {7.0, 0.00001, 300.0};
  AfanasyevAItSeidelMethodMPI task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());

  if (rank == 0) {
    auto output = task.GetOutput();
    EXPECT_EQ(output.size(), static_cast<std::size_t>(7));
  }
}

}  // namespace afanasyev_a_it_seidel_method
