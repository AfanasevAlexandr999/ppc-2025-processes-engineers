#include <gtest/gtest.h>
#include <mpi.h>

#include <cmath>
#include <vector>

#include "afanasyev_a_it_seidel_method/common/include/common.hpp"
#include "afanasyev_a_it_seidel_method/mpi/include/ops_mpi.hpp"
#include "afanasyev_a_it_seidel_method/seq/include/ops_seq.hpp"

namespace afanasyev_a_it_seidel_method {

TEST(AfanasyevAItSeidelMethodFuncTests, SeidelMethodSEQ_SmallSystem) {
  InType input = {3.0, 0.001, 100.0};
  AfanasyevAItSeidelMethodSEQ task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());

  auto output = task.GetOutput();
  EXPECT_EQ(output.size(), static_cast<size_t>(3));
}

TEST(AfanasyevAItSeidelMethodFuncTests, SeidelMethodSEQ_MediumSystem) {
  InType input = {5.0, 0.0001, 200.0};
  AfanasyevAItSeidelMethodSEQ task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());

  auto output = task.GetOutput();
  EXPECT_EQ(output.size(), static_cast<size_t>(5));
}

TEST(AfanasyevAItSeidelMethodFuncTests, SeidelMethodSEQ_LargeSystem) {
  InType input = {7.0, 0.00001, 300.0};
  AfanasyevAItSeidelMethodSEQ task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());

  auto output = task.GetOutput();
  EXPECT_EQ(output.size(), static_cast<size_t>(7));
}

TEST(AfanasyevAItSeidelMethodFuncTests, SeidelMethodMPI_SmallSystem) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  InType input = {3.0, 0.001, 100.0};
  AfanasyevAItSeidelMethodMPI task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());

  if (rank == 0) {
    auto output = task.GetOutput();
    EXPECT_EQ(output.size(), static_cast<size_t>(3));
  }
}

TEST(AfanasyevAItSeidelMethodFuncTests, SeidelMethodMPI_MediumSystem) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  InType input = {5.0, 0.0001, 200.0};
  AfanasyevAItSeidelMethodMPI task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());

  if (rank == 0) {
    auto output = task.GetOutput();
    EXPECT_EQ(output.size(), static_cast<size_t>(5));
  }
}

TEST(AfanasyevAItSeidelMethodFuncTests, SeidelMethodMPI_LargeSystem) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  InType input = {7.0, 0.00001, 300.0};
  AfanasyevAItSeidelMethodMPI task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());
  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());

  if (rank == 0) {
    auto output = task.GetOutput();
    EXPECT_EQ(output.size(), static_cast<size_t>(7));
  }
}

}  // namespace afanasyev_a_it_seidel_method
