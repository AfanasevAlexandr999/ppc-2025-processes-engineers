#include <gtest/gtest.h>
#include <mpi.h>

#include <chrono>

#include "afanasyev_a_it_seidel_method/common/include/common.hpp"
#include "afanasyev_a_it_seidel_method/mpi/include/ops_mpi.hpp"
#include "afanasyev_a_it_seidel_method/seq/include/ops_seq.hpp"

namespace afanasyev_a_it_seidel_method {

TEST(AfanasyevAItSeidelMethodPerfTests, SEQ_Performance) {
  InType input = {100.0, 0.0001, 1000.0};
  AfanasyevAItSeidelMethodSEQ task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());

  auto start = std::chrono::high_resolution_clock::now();
  EXPECT_TRUE(task.Run());
  auto end = std::chrono::high_resolution_clock::now();

  EXPECT_TRUE(task.PostProcessing());

  auto output = task.GetOutput();
  EXPECT_EQ(output.size(), 100u);
}

TEST(AfanasyevAItSeidelMethodPerfTests, SEQ_SmallSystem) {
  InType input = {50.0, 0.001, 500.0};
  AfanasyevAItSeidelMethodSEQ task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());

  auto start = std::chrono::high_resolution_clock::now();
  EXPECT_TRUE(task.Run());
  auto end = std::chrono::high_resolution_clock::now();

  EXPECT_TRUE(task.PostProcessing());

  auto output = task.GetOutput();
  EXPECT_EQ(output.size(), 50u);
}

TEST(AfanasyevAItSeidelMethodPerfTests, MPI_Performance) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  InType input = {100.0, 0.0001, 1000.0};
  AfanasyevAItSeidelMethodMPI task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());

  MPI_Barrier(MPI_COMM_WORLD);
  auto start = std::chrono::high_resolution_clock::now();

  EXPECT_TRUE(task.Run());

  MPI_Barrier(MPI_COMM_WORLD);
  auto end = std::chrono::high_resolution_clock::now();

  EXPECT_TRUE(task.PostProcessing());

  if (rank == 0) {
    auto output = task.GetOutput();
    EXPECT_EQ(output.size(), 100u);
  }
}

TEST(AfanasyevAItSeidelMethodPerfTests, MPI_SmallSystem) {
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  InType input = {50.0, 0.001, 500.0};
  AfanasyevAItSeidelMethodMPI task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());

  MPI_Barrier(MPI_COMM_WORLD);
  auto start = std::chrono::high_resolution_clock::now();

  EXPECT_TRUE(task.Run());

  MPI_Barrier(MPI_COMM_WORLD);
  auto end = std::chrono::high_resolution_clock::now();

  EXPECT_TRUE(task.PostProcessing());

  if (rank == 0) {
    auto output = task.GetOutput();
    EXPECT_EQ(output.size(), 50u);
  }
}

}  // namespace afanasyev_a_it_seidel_method
