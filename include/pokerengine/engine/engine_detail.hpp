//
// Created by user on 3/29/25.
//

#ifndef POKERENGINE_ENGINE_DETAIL_HPP
#define POKERENGINE_ENGINE_DETAIL_HPP

#include <cstdint>
#include <stdexcept>

#include "enums.hpp"
#include "player.hpp"
#include "pokerengine.hpp"

namespace pokerengine {
class engine_error : public exception {
  public:
  using exception::exception;

  protected:
  std::string class_name_ = "engine_error";
};

class engine_traits {
  public:
  engine_traits(uint16_t sb_bet, uint16_t bb_bet, uint8_t bb_mult, uint32_t min_raise = -1)
          : sb_bet_{ sb_bet }, bb_bet_{ bb_bet }, bb_mult_{ bb_mult } {
    min_raise_ = min_raise > 0 ? min_raise : bb_bet_ * 2;
  }

  [[nodiscard]] auto get_sb_bet() const noexcept -> uint16_t {
    return sb_bet_;
  }

  [[nodiscard]] auto get_bb_bet() const noexcept -> uint16_t {
    return bb_bet_;
  }

  [[nodiscard]] auto get_bb_mult() const noexcept -> uint8_t {
    return bb_mult_;
  }

  [[nodiscard]] auto get_min_raise() const noexcept -> uint32_t {
    return min_raise_;
  }

  auto set_sb_bet(uint16_t value) noexcept -> void {
    sb_bet_ = value;
  }

  auto set_bb_bet(uint16_t value) noexcept -> void {
    bb_bet_ = value;
  }

  auto set_bb_mult(uint8_t value) noexcept -> void {
    bb_mult_ = value;
  }

  auto set_min_raise(uint32_t value) noexcept -> void {
    min_raise_ = value;
  }

  private:
  uint16_t sb_bet_;
  uint16_t bb_bet_;
  uint8_t bb_mult_;

  uint32_t min_raise_;
};

template < uint8_t A = 0, uint8_t B = 1 >
  requires(A >= 0 && B > 0 && A < B)
class engine {
  public:
  engine() = delete;

  explicit engine(const engine_traits &engine_traits) : engine_traits_{ engine_traits } {
  }

  [[nodiscard]] auto get_current() const noexcept -> enums::position {
    return current_;
  }

  [[nodiscard]] auto get_current_player() const noexcept -> enums::position {
    return get_player(static_cast< uint8_t >(get_current()));
  }

  [[nodiscard]] auto get_players() const noexcept -> std::vector< player > {
    return players_;
  }

  auto add_player(uint32_t stack, const std::string &id) -> void {
    for (const auto &player : get_players()) {
      if (player.id == id) {
        throw engine_error{ "Player already in the game" };
      }
    }
    if (stack < engine_traits_.get_bb_bet() * engine_traits_.get_bb_mult()) {
      throw engine_error{ "Player stack less than game minimal stacksize" };
    }

    add_player(player{ .stack = stack, .bet = 0, .round_bet = 0, .state = enums::state::init, .id = id });
  }

  protected:
  [[nodiscard]] auto get_players() noexcept -> std::vector< player > & {
    return players_;
  }

  [[nodiscard]] auto get_player(uint8_t index) -> player & {
    if (index < 0 || index > players_.size()) {
      throw std::length_error{ "Invalid index" };
    }

    return *(players_.begin() + index);
  }

  auto add_player(const player &player) -> void {
    players_.push_back(player);
  }

  auto set_players(const std::vector< player > &players) noexcept -> void {
    players_ = players;
  }

  enums::position current_{ enums::position::sb };
  enums::round round_{ enums::round::preflop };
  bool flop_dealt_{ false };
  std::vector< player > players_;

  private:
  engine_traits engine_traits_;
};
} // namespace pokerengine
#endif // POKERENGINE_ENGINE_DETAIL_HPP
