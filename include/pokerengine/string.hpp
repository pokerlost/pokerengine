#ifndef POKERENGINE_STRING_HPP
#define POKERENGINE_STRING_HPP

#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "pokerengine.hpp"

namespace pokerengine::string {
template < typename T >
auto copy(const std::vector< T > &value) -> std::string {
  std::ostringstream stream;
  stream << "[";
  for (size_t index = 0; index < value.size() - 1; index++) {
    stream << std::string{ value[index] } << ", ";
  }
  stream << std::string{ value.back() };
  stream << "]";

  return stream.str();
}
} // namespace pokerengine::string
#endif // POKERENGINE_STRING_HPP
