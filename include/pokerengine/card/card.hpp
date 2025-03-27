//
// Created by copper_boy on 7/4/23.
//

#ifndef POKERENGINE_CARD_HPP
#define POKERENGINE_CARD_HPP

#include <algorithm>
#include <compare>
#include <cstdint>
#include <limits>
#include <random>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include <magic_enum.hpp>

#include "constants.hpp"
#include "enums.hpp"
#include "pokerengine.hpp"
#include "string.hpp"
#include "types.hpp"

namespace pokerengine {
template < typename EnumType, typename ValueType, typename BitSetType >
  requires(std::is_enum< EnumType >::value && std::numeric_limits< ValueType >::is_integer &&
           std::numeric_limits< BitSetType >::is_integer)
class representation {
  public:
  explicit representation< EnumType, ValueType, BitSetType >(EnumType value) {
    value_ = static_cast< ValueType >(value);
  }

  explicit representation< EnumType, ValueType, BitSetType >(char value) {
    value_ = static_cast< ValueType >(magic_enum::enum_cast< EnumType >(std::string{ value }).value());
  }

  auto operator<=>(const representation &other) const noexcept -> std::strong_ordering = default;

  explicit operator std::string() const {
    return magic_enum::enum_name(get_enum()).data();
  }

  [[nodiscard]] auto as_bitset() const noexcept -> BitSetType {
    return BitSetType{ 1 } << get_value();
  }

  [[nodiscard]] auto get_value() const noexcept -> ValueType {
    return value_;
  }

  [[nodiscard]] auto get_enum() const noexcept -> EnumType {
    return static_cast< EnumType >(get_value());
  }

  private:
  ValueType value_;
};

using rank = representation< enums::rank, int, int16_t >;
using suit = representation< enums::suit, int, int16_t >;

class card {
  public:
  card() = delete;

  card(const rank &rank_value, const suit &suit_value) noexcept
          : card_{ static_cast< uint8_t >(
                            rank_value.get_value() + (suit_value.get_value() * constants::RANKS)) } {
  }

  card(enums::rank rank_value, enums::suit suit_value) : card{ rank{ rank_value }, suit{ suit_value } } {
  }

  explicit card(uint8_t value) : card_{ value } {
    if (value != std::clamp(value, constants::CARD_INDEX_MIN, constants::CARD_INDEX_MAX)) {
      throw std::runtime_error{ "Invalid argument value for card" };
    }
  }

  explicit card(std::string_view value) : card{ rank{ value[0] }, suit{ value[1] } } {
  }

  auto operator<=>(const card &other) const noexcept -> std::strong_ordering {
    std::strong_ordering result = get_card() % constants::RANKS <=> other.get_card() % constants::RANKS;
    if (result != std::strong_ordering::equal) {
      return result;
    }

    return (get_card() / constants::SUITS) <=> (other.get_card() / constants::SUITS);
  }

  auto operator==(const card &other) const noexcept -> bool {
    return get_card() == other.get_card();
  }

  explicit operator std::string() const {
    return std::string{ get_rank() } + std::string{ get_suit() };
  }

  [[nodiscard]] auto as_bitset() const noexcept -> types::bit_set {
    return types::bit_set{ 1 } << card_;
  }

  [[nodiscard]] auto get_card() const noexcept -> uint8_t {
    return card_;
  }

  [[nodiscard]] auto get_rank() const noexcept -> rank {
    return rank{ enums::rank(uint8_t(card_ % constants::RANKS)) };
  }

  [[nodiscard]] auto get_suit() const noexcept -> suit {
    return suit{ enums::suit(uint8_t(card_ / constants::RANKS)) };
  }

  private:
  uint8_t card_;
};

class card_set {
  public:
  card_set() = default;

  card_set(const std::initializer_list< const card > &values) {
    for (auto const &value : values) {
      cards_ |= value.as_bitset();
    }
  }

  explicit card_set(uint64_t bitset) : cards_{ bitset } {
    if (cards_ > constants::CARD_SET_FULL) {
      throw std::runtime_error{ "Card size reached" };
    }
  }

  explicit card_set(const std::span< const card > &sp) noexcept {
    for (auto &&c : sp) {
      cards_ |= c.as_bitset();
    }
  }

  explicit card_set(std::string_view value) {
    if (value.size() % 2 != 0) {
      throw std::runtime_error{ "String size must be divisible by 2" };
    }

    for (size_t index = 0; index < value.size(); index += 2) {
      cards_ |= card(value.substr(index, 2)).as_bitset();
    }
  }

  [[nodiscard]] auto size() const noexcept -> size_t {
    return std::popcount(get_cards());
  }

  [[nodiscard]] auto contains(const card &value) const noexcept -> bool {
    return (get_cards() & value.as_bitset()) != 0;
  }

  [[nodiscard]] auto contains(const card_set &value) const noexcept -> bool {
    return (get_cards() | value.get_cards()) == get_cards();
  }

  [[nodiscard]] auto combine(const card &value) const noexcept -> card_set {
    return card_set{ get_cards() | value.as_bitset() };
  }

  [[nodiscard]] auto combine(const card_set &value) const noexcept -> card_set {
    return card_set{ get_cards() | value.get_cards() };
  }

  auto clear() noexcept -> void {
    set_cards(0);
  }

  auto fill() noexcept -> void {
    set_cards(constants::CARD_SET_FULL);
  }

  auto insert(const card &value) noexcept -> void {
    set_cards(get_cards() | value.as_bitset());
  }

  auto join(const card_set &value) noexcept -> void {
    set_cards(get_cards() | value.get_cards());
  }

  auto remove(const card &value) noexcept -> void {
    set_cards(get_cards() ^ value.as_bitset());
  }

  auto remove(const card_set value) noexcept -> void {
    set_cards(get_cards() ^ value.get_cards());
  }

  [[nodiscard]] auto get_cards() const noexcept -> uint64_t {
    return cards_;
  }

  auto set_cards(uint64_t value) noexcept -> void {
    cards_ = value;
  }

  private:
  uint64_t cards_ = 0;
};

class card_generator {
  public:
  explicit card_generator(uint16_t seed = 1927)
          : rng_(seed), dist_rank_(0, constants::RANKS), dist_suit_(0, constants::SUITS) {
  }

  [[nodiscard]] auto generate() -> card {
    return card{ rank{ enums::rank{ uint8_t(dist_rank_(rng_)) } },
                 suit{ enums::suit{ static_cast< uint8_t >(dist_suit_(rng_)) } } };
  }

  [[nodiscard]] auto generate_v(uint8_t n) -> std::vector< card > {
    if (n > ((constants::RANKS) * (constants::SUITS))) {
      throw std::runtime_error("N greater than number of specified unique cards");
    }

    std::vector< uint8_t > cards;
    cards.reserve((constants::RANKS) * (constants::SUITS));
    for (uint8_t i = 0; i <= constants::RANKS - 1; ++i) {
      for (uint8_t j = 0; j <= constants::SUITS - 1; ++j) {
        cards.emplace_back(static_cast< uint8_t >(i + j * constants::RANKS));
      }
    }
    std::shuffle(cards.begin(), cards.end(), rng_);
    cards.resize(n);

    std::vector< card > result;
    result.reserve(n);

    std::for_each(cards.cbegin(), cards.cend(), [&](uint8_t index) -> void { result.emplace_back(index); });
    return result;
  }

  private:
  std::mt19937 rng_;
  std::uniform_int_distribution< int > dist_rank_;
  std::uniform_int_distribution< int > dist_suit_;
};
} // namespace pokerengine

#endif // POKERENGINE_CARD_HPP
