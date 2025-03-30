#ifndef POKERENGINE_HAND_HPP
#define POKERENGINE_HAND_HPP

#include <array>
#include <compare>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "card/card.hpp"
#include "constants.hpp"
#include "pokerengine.hpp"
#include "types.hpp"

namespace pokerengine {
template < typename T >
using is_card = std::is_same< T, typename pokerengine::card >;

template < typename T >
const bool is_card_v = is_card< T >::value;

template < typename T >
using is_rank = std::is_same< T, typename pokerengine::rank >;

template < typename T >
using is_card_or_rank = std::disjunction< is_card< T >, is_rank< T > >;

template < typename T >
inline const bool is_card_or_rank_v = is_card_or_rank< T >::value;

template < typename T, bool allow_duplicates, size_t N, std::enable_if_t< is_card_or_rank_v< T >, int > = 0 >
class hand_helper {
  public:
  hand_helper() = delete;

  template < std::enable_if_t< N == constants::HAND_SIZE, int > = 0 >
  hand_helper(T first, T second) noexcept(allow_duplicates) : value_{ first, second } {
    if (!allow_duplicates && (static_cast< card >(static_cast< card >(static_cast< card >(first))) == second)) {
      throw std::runtime_error{ "Tried to create hand with equal input" };
    }
  }

  template < std::enable_if_t< is_card_v< T > && N == constants::HAND_SIZE, int > = 0 >
  hand_helper(uint8_t first, uint8_t second) noexcept(allow_duplicates)
          : hand_helper{ T{ first }, T{ second } } {
  }

  template < std::enable_if_t< N == constants::HAND_SIZE, int > = 0 >
  explicit hand_helper(std::string_view str)
          : hand_helper(T{ str.substr(0 * char_count, char_count) },
                        T{ str.substr(1 * char_count, char_count) }) {
    if (str.size() != constants::HAND_SIZE * char_count) {
      throw std::runtime_error{ "Tried to create a hand with a string of wrong size" };
    }
  }

  auto operator<=>(const hand_helper &other) const noexcept -> std::strong_ordering = default;

  template < std::enable_if_t< N == constants::HAND_SIZE, int > = 0 >
  explicit operator std::string() const {
    auto iterable = get_value();
    return std::string{ iterable.front() } + std::string{ iterable.back() };
  }

  template < typename Indices = std::make_index_sequence< N > >
  [[nodiscard]] auto as_bitset() const noexcept -> types::bit_set {
    return as_bitset_impl(Indices());
  }

  template < typename U = T, std::enable_if_t< is_card_v< U >, int > = 0 >
  [[nodiscard]] auto as_cardset() const noexcept -> card_set {
    return card_set{ get_value() };
  }

  auto get_value() const noexcept -> std::array< T, N > {
    return value_;
  }

  static auto get_size() noexcept -> size_t {
    return N;
  }

  private:
  static const auto char_count = is_card_v< T > ? 2 : 1;

  template < size_t... I >
  auto as_bitset_impl(std::index_sequence< I... >) const noexcept -> types::bit_set {
    return static_cast< types::bit_set >((get_value()[I].as_bitset() | ...));
  }

  std::array< T, N > value_;
};

using hand = hand_helper< card, false, constants::HAND_SIZE >;
} // namespace pokerengine
#endif // POKERENGINE_HAND_HPP
