#pragma once

#include <string>
#include <tuple>

#include "task/include/task.hpp"

namespace afanasyev_a_elem_vec_avg {

using InType = int;
using OutType = int;
using TestType = std::tuple<int, std::string>;
using BaseTask = ppc::task::Task<InType, OutType>;

}  // namespace afanasyev_a_elem_vec_avg
