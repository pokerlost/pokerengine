#ifndef POKERENGINE_ENUMS_HPP
#define POKERENGINE_ENUMS_HPP

#include <cstdint>

#include <magic_enum/magic_enum.hpp>

#include "pokerengine.hpp"

namespace pokerengine::enums {
enum class action : uint8_t {
  fold = 0,
  check = 1,
  call = 2,
  bet = 3,
  raise = 4,
};

enum class combination : uint8_t {
  no_pair = 0,
  one_pair = 1,
  two_pair = 2,
  three_of_a_kind = 3,
  straight = 4,
  flush = 5,
  full_house = 6,
  four_of_a_kind = 7,
  straight_flush = 8,
};

enum class position : uint8_t {
  sb = 0,
  bb = 1,
  utg = 2,
  lwj = 3,
  hij = 4,
  cof = 5,
  btn = 6,
};

enum class rank : uint8_t {
  two = 0,
  three = 1,
  four = 2,
  five = 3,
  six = 4,
  seven = 5,
  eight = 6,
  nine = 7,
  ten = 8,
  jack = 9,
  queen = 10,
  king = 11,
  ace = 12,
};

enum class round : uint8_t {
  preflop = 0,
  flop = 1,
  turn = 2,
  river = 3,
  showdown = 4,
};

enum class state : uint8_t {
  init = 0,
  out = 1,
  alive = 2,
  allin = 3,
};

enum class suit : uint8_t {
  clubs = 0,
  diamonds = 1,
  hearts = 2,
  spades = 3,
};
} // namespace pokerengine::enums

template <>
[[maybe_unused]] constexpr magic_enum::customize::customize_t
                magic_enum::customize::enum_name< pokerengine::enums::rank >(
                                pokerengine::enums::rank value) noexcept {
  switch (value) {
  case pokerengine::enums::rank::two: {
    return "2";
  }
  case pokerengine::enums::rank::three: {
    return "3";
  }
  case pokerengine::enums::rank::four: {
    return "4";
  }
  case pokerengine::enums::rank::five: {
    return "5";
  }
  case pokerengine::enums::rank::six: {
    return "6";
  }
  case pokerengine::enums::rank::seven: {
    return "7";
  }
  case pokerengine::enums::rank::eight: {
    return "8";
  }
  case pokerengine::enums::rank::nine: {
    return "9";
  }
  case pokerengine::enums::rank::ten: {
    return "t";
  }
  case pokerengine::enums::rank::jack: {
    return "j";
  }
  case pokerengine::enums::rank::queen: {
    return "q";
  }
  case pokerengine::enums::rank::king: {
    return "k";
  }
  case pokerengine::enums::rank::ace: {
    return "a";
  }
  default: {
    return default_tag;
  }
  }
}

template <>
[[maybe_unused]] constexpr magic_enum::customize::customize_t
                magic_enum::customize::enum_name< pokerengine::enums::suit >(
                                pokerengine::enums::suit value) noexcept {
  switch (value) {
  case pokerengine::enums::suit::clubs: {
    return "c";
  }
  case pokerengine::enums::suit::diamonds: {
    return "d";
  }
  case pokerengine::enums::suit::hearts: {
    return "h";
  }
  case pokerengine::enums::suit::spades: {
    return "s";
  }
  default: {
    return default_tag;
  }
  }
}
#endif // POKERENGINE_ENUMS_HPP
