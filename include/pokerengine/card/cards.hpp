//
// Created by copper_boy on 7/4/23.
//

#ifndef POKERENGINE_CARDS_HPP
#define POKERENGINE_CARDS_HPP

#include <compare>
#include <initializer_list>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "card/card.hpp"
#include "card/hand.hpp"
#include "constants.hpp"
#include "pokerengine.hpp"

namespace pokerengine {
class cards {
  public:
  cards() = delete;

  cards(const std::vector< std::string > &board, const std::vector< std::string > &hands)
          : board_(board.begin(), board.end()), hands_(hands.begin(), hands.end()) {
    if (board_.size() > constants::BOARD_SIZE) {
      throw std::length_error{ "Board size in texas holdem poker must be less or equal than 5" };
    }

    if (board_.size() + hands_.size() !=
        std::set(board_.cbegin(), board_.cend()).size() + std::set(hands_.cbegin(), hands_.cend()).size()) {
      throw std::logic_error{ "Cards not unique" };
    }
  }

  explicit cards(const std::vector< card > &cards) {
    std::span< const card > span{ cards.data(), cards.size() };

    for (size_t index = 0; index < constants::BOARD_SIZE; index++) {
      board_.push_back(span[index]);
    }

    for (size_t index = 0; index < (cards.size() - constants::BOARD_SIZE) / pokerengine::hand::get_size();
         index++) {
      hands_.emplace_back(
                      span[2ull * index + constants::BOARD_SIZE],
                      span[2ull * index + constants::BOARD_SIZE + 1]);
    }
  }

  auto operator<=>(const cards &other) const noexcept -> std::strong_ordering = default;

  explicit operator std::string() const {
    using namespace std::literals;

    return "Cards<board="s + string::copy(get_board()) + ", "s + "hands=" + string::copy(get_hands()) + ">"s;
  }

  [[nodiscard]] auto get_board_n(uint8_t n) -> std::vector< card > {
    if (n > constants::BOARD_SIZE) {
      throw std::runtime_error{ "Index greater than " + std::to_string(constants::BOARD_SIZE) +
                                " i.e. the board size" };
    }

    std::span< card > span{ board_.data(), n };
    return std::vector< card >{ span.begin(), span.end() };
  }

  [[nodiscard]] auto get_board() const noexcept -> std::vector< card > {
    return board_;
  }

  [[nodiscard]] auto get_hands() const noexcept -> std::vector< hand > {
    return hands_;
  }

  private:
  std::vector< card > board_;
  std::vector< hand > hands_;
};
} // namespace pokerengine

#endif // POKERENGINE_CARDS_HPP
