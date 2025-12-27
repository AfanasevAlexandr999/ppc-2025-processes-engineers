#pragma once

#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace afanasyev_a_batch_sort {

using InType = int;
using OutType = int;
using TestType = std::tuple<int, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace afanasyev_a_batch_sort
