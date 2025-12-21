#include <gtest/gtest.h>
#include <mpi.h>

#include <cstddef>

#include "afanasyev_a_it_seidel_method/common/include/common.hpp"
#include "afanasyev_a_it_seidel_method/mpi/include/ops_mpi.hpp"
#include "afanasyev_a_it_seidel_method/seq/include/ops_seq.hpp"

namespace afanasyev_a_it_seidel_method {

TEST(AfanasyevAItSeidelMethodPerfTests, SeqPerformance) {
  InType input = {100.0, 0.001, 5000.0};
  AfanasyevAItSeidelMethodSEQ task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());

  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());

  auto output = task.GetOutput();
  EXPECT_EQ(output.size(), static_cast<std::size_t>(100));
}

TEST(AfanasyevAItSeidelMethodPerfTests, SeqSmallSystem) {
  InType input = {50.0, 0.001, 2000.0};
  AfanasyevAItSeidelMethodSEQ task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());

  EXPECT_TRUE(task.Run());
  EXPECT_TRUE(task.PostProcessing());

  auto output = task.GetOutput();
  EXPECT_EQ(output.size(), static_cast<std::size_t>(50));
}

TEST(AfanasyevAItSeidelMethodPerfTests, MpiPerformance) {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  InType input = {100.0, 0.001, 5000.0};
  AfanasyevAItSeidelMethodMPI task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());

  MPI_Barrier(MPI_COMM_WORLD);
  EXPECT_TRUE(task.Run());
  MPI_Barrier(MPI_COMM_WORLD);

  EXPECT_TRUE(task.PostProcessing());

  if (rank == 0) {
    auto output = task.GetOutput();
    EXPECT_EQ(output.size(), static_cast<std::size_t>(100));
  }
}

TEST(AfanasyevAItSeidelMethodPerfTests, MpiSmallSystem) {
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  InType input = {50.0, 0.001, 2000.0};
  AfanasyevAItSeidelMethodMPI task(input);

  EXPECT_TRUE(task.Validation());
  EXPECT_TRUE(task.PreProcessing());

  MPI_Barrier(MPI_COMM_WORLD);
  EXPECT_TRUE(task.Run());
  MPI_Barrier(MPI_COMM_WORLD);

  EXPECT_TRUE(task.PostProcessing());

  if (rank == 0) {
    auto output = task.GetOutput();
    EXPECT_EQ(output.size(), static_cast<std::size_t>(50));
  }
}

}  // namespace afanasyev_a_it_seidel_method