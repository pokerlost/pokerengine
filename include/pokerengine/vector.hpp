//
// Created by copper_boy on 10/9/23.
//

#ifndef POKERENGINE_VECTOR_HPP
#define POKERENGINE_VECTOR_HPP

#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <vector>

#include "pokerengine.hpp"


namespace pokerengine {
template < typename T >
  requires(std::is_integral_v< T >)
auto operator+=(std::vector< T > &lhs, const std::vector< T > &rhs) -> std::vector< T > & {
  std::vector< T > rhs_{ rhs.begin(), rhs.end() };

  if (lhs.size() > rhs_.size()) {
    while (lhs.size() > rhs_.size()) {
      rhs_.push_back(T{});
    }
  } else {
    while (rhs_.size() > lhs.size()) {
      lhs.push_back(T{});
    }
  }

  for (size_t index = 0; index < lhs.size(); index++) {
    lhs[index] += rhs_[index];
  }

  return lhs;
}

template < typename T >
auto operator+(const std::vector< T > &lhs, const std::vector< T > &rhs) -> std::vector< T > {
  std::vector< T > result(lhs.cbegin(), lhs.cend());
  result += rhs;

  return result;
}
} // namespace pokerengine

#endif // POKERENGINE_VECTOR_HPP
