//
// Created by copper_boy on 7/4/23.
//

#ifndef POKERENGINE_BITS_HPP
#define POKERENGINE_BITS_HPP

#include <bit>
#include <cstdint>

#include "constants.hpp"
#include "pokerengine.hpp"

namespace pokerengine::bits {
auto cross_idx_low16(uint16_t mask) -> uint8_t {
  return mask == 0 ? 0 : static_cast< uint8_t >(std::countr_zero(mask));
}

auto cross_idx_high16(uint16_t mask) -> uint8_t {
  return mask == 0 ? 0 : static_cast< uint8_t >(15 - std::countl_zero(mask));
}
} // namespace pokerengine::bits

#endif // POKERENGINE_BITS_HPP
