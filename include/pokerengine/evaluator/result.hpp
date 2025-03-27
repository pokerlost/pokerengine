//
// Created by copper_boy on 7/4/23.
//

#ifndef POKERENGINE_RESULT_HPP
#define POKERENGINE_RESULT_HPP

#include <compare>
#include <cstdint>
#include <string>

#include "bits.hpp"
#include "card/card.hpp"
#include "constants.hpp"
#include "enums.hpp"
#include "pokerengine.hpp"
#include "types.hpp"

namespace pokerengine {
class result {
  public:
  result(enums::combination type, uint8_t major, uint8_t minor, uint16_t kickers) noexcept
          : result_{ static_cast< uint32_t >(
                            (static_cast< uint8_t >(type) << constants::OFFSET_TYPE) |
                            (major << constants::OFFSET_MAJOR) | (minor << constants::OFFSET_MINOR) |
                            (kickers & constants::MASK_RANKS)) } {
  }

  result(uint8_t type, uint8_t major, uint8_t minor, uint16_t kickers) noexcept
          : result_{ static_cast< uint32_t >(
                            (type << constants::OFFSET_TYPE) | (major << constants::OFFSET_MAJOR) |
                            (minor << constants::OFFSET_MINOR) | (kickers & constants::MASK_RANKS)) } {
  }

  auto operator<=>(const result &other) const -> std::strong_ordering = default;

  explicit operator std::string() const {
    auto result = enums::combination(get_type());

    switch (result) {
    case enums::combination::no_pair:
    case enums::combination::one_pair:
    case enums::combination::three_of_a_kind:
    case enums::combination::four_of_a_kind:
    case enums::combination::two_pair:
    case enums::combination::flush: {
      return as_string_base() + ", kicker(s): " + str_kickers();
    }
    default: {
      return as_string_base();
    }
    }
  }
  [[nodiscard]] auto get_result() const noexcept -> uint32_t {
    return result_;
  }

  [[nodiscard]] auto get_type() const noexcept -> int8_t {
    return static_cast< int8_t >(get_result() >> constants::OFFSET_TYPE);
  }

  [[nodiscard]] auto get_major_rank() const noexcept -> rank {
    return rank{ enums::rank(constants::MASK_RANKS_NUMBER & ((get_result() >> constants::OFFSET_MAJOR) - 0)) };
  }

  [[nodiscard]] auto get_minor_rank() const noexcept -> rank {
    return rank{ enums::rank(constants::MASK_RANKS_NUMBER & ((get_result() >> constants::OFFSET_MINOR) - 0)) };
  }

  [[nodiscard]] auto get_kickers() const noexcept -> uint32_t {
    return (constants::MASK_RANKS & get_result());
  }

  [[nodiscard]] auto as_bitset() const noexcept -> types::bit_set {
    return get_result();
  }

  private:
  [[nodiscard]] auto as_string_base() const -> std::string {
    auto result = enums::combination{ get_type() };
    auto index = static_cast< uint8_t >(result);

    switch (result) {
    case enums::combination::no_pair: {
      return std::string{ constants::COMBINATIONS[index] } + std::string{ " " } +
                      std::string{ rank{ enums::rank{ bits::cross_idx_high16(get_kickers()) } } };
    }
    case enums::combination::one_pair:
    case enums::combination::three_of_a_kind: {
      return std::string{ constants::COMBINATIONS[index] } + std::string{ " " } +
                      std::string{ get_major_rank() };
    }
    case enums::combination::four_of_a_kind: {
      return std::string{ constants::COMBINATIONS[index] } + std::string{ " " } +
                      std::string{ get_major_rank() };
    }
    case enums::combination::two_pair: {
      return std::string{ constants::COMBINATIONS[index] } + std::string{ " " } +
                      std::string{ get_major_rank() } + std::string{ " and " } +
                      std::string{ get_minor_rank() };
    }
    case enums::combination::full_house: {
      return std::string{ constants::COMBINATIONS[index] } + std::string{ " " } +
                      std::string{ get_major_rank() } + std::string{ " full of " } +
                      std::string{ get_minor_rank() };
    }
    case enums::combination::straight:
    case enums::combination::straight_flush: {
      return std::string{ constants::COMBINATIONS[index] } + std::string{ " " } +
                      std::string{ get_major_rank() } + std::string{ " high" };
    }
    case enums::combination::flush: {
      return std::string{ constants::COMBINATIONS[index] } + std::string{ " " } +
                      std::string{ rank{ enums::rank{ bits::cross_idx_high16(get_kickers()) } } } +
                      std::string{ " high" };
    }
    default: {
      throw std::runtime_error{ "Invalid evaluation result" };
    }
    }
  }

  [[nodiscard]] auto str_kickers() const -> std::string {
    std::string string;
    uint16_t k = get_kickers();

    int8_t index = constants::RANKS;
    for (uint64_t mask = uint64_t(1) << constants::RANKS; mask; mask >>= 1, index--) {
      if (k & mask) {
        string += std::string{ rank(static_cast< enums::rank >(index)) } + " ";
      }
    }
    string.pop_back();

    return string;
  }

  uint32_t result_;
};
} // namespace pokerengine

#endif // POKERENGINE_RESULT_HPP
