#ifndef POKERENGINE_ACTION_HPP
#define POKERENGINE_ACTION_HPP

#include <algorithm>
#include <stdexcept>

#include <magic_enum/magic_enum.hpp>
#include <utility>

#include "enums.hpp"
#include "player.hpp"
#include "pokerengine.hpp"

namespace pokerengine {
namespace exceptions {
class actions_error : public exception {
  public:
  using exception::exception;

  protected:
  std::string class_name_ = "actions_error";
};
} // namespace exceptions

struct player_action {
  uint32_t amount;
  enums::action action;
  enums::position position;

  auto operator<=>(const player_action &other) const noexcept -> std::strong_ordering = delete;
  auto operator==(const player_action &other) const noexcept -> bool = default;

  explicit operator std::string() const {
    using namespace std::literals;

    return "PlayerAction<amount="s + std::to_string(amount) + ", action="s +
                    std::string{ magic_enum::enum_name(action) } + ", position="s +
                    std::string{ magic_enum::enum_name(position) } + ">"s;
  }
};

auto is_no_actions_available(enums::round round, enums::state state) noexcept -> bool {
  return (state == enums::state::out || state == enums::state::allin) || round == enums::round::showdown;
}

auto is_check_available(uint32_t highest_bet, uint32_t bet) {
  return !highest_bet || bet == highest_bet;
}

auto is_bet_available(uint32_t bb_bet, uint32_t highest_bet, uint32_t stack) noexcept -> bool {
  return !highest_bet && stack > bb_bet;
}

auto is_call_available(uint32_t highest_bet, uint32_t bet, uint32_t stack) noexcept -> bool {
  if (bet + stack <= highest_bet) {
    return true;
  }

  return highest_bet && (bet < highest_bet && (bet + stack) >= highest_bet);
}

auto is_raise_available(enums::state state, uint32_t highest_bet, uint32_t min_raise, uint32_t bet, uint32_t stack)
                -> bool {
  if ((bet + stack) <= highest_bet) {
    return false;
  }

  return (state == enums::state::init || state == enums::state::alive) ||
                  (bet < highest_bet && (highest_bet - bet >= min_raise));
}

auto get_possible_actions(
                enums::round round,
                enums::position player,
                enums::state state,
                uint16_t bb_bet,
                uint32_t min_raise,
                uint32_t highest_bet,
                uint32_t bet,
                uint32_t stack) -> std::vector< player_action > {
  if (is_no_actions_available(round, state)) {
    throw exceptions::actions_error{ "No actions available, wrong game state or player state" };
  }

  std::vector< player_action > actions{};
  if (is_check_available(highest_bet, bet)) {
    actions.emplace_back(player_action{ 0, enums::action::check, player });
  } else {
    actions.emplace_back(player_action{ 0, enums::action::fold, player });
  }
  if (is_bet_available(bb_bet, highest_bet, stack)) {
    actions.emplace_back(stack, enums::action::bet, player);
  } else {
    if (is_raise_available(state, highest_bet, min_raise, bet, stack)) {
      actions.emplace_back(stack, enums::action::raise, player);
    }
  }
  if (is_call_available(highest_bet, bet, stack)) {
    actions.emplace_back(highest_bet - bet, enums::action::call, player);
  }

  return actions;
}

auto is_action_allowed(
                const std::vector< player_action > &actions,
                enums::position player,
                enums::position position,
                enums::action action,
                uint32_t amount,
                uint32_t min_raise) -> bool {
  return position == player &&
                  (std::find_if(actions.cbegin(), actions.cend(), [&](const auto &element) -> bool {
                     return (element.action == action && element.amount == amount) ||
                                     (action == enums::action::bet && element.action == enums::action::bet &&
                                      amount < element.amount && amount >= min_raise) ||
                                     (action == enums::action::raise &&
                                      element.action == enums::action::raise && amount < element.amount &&
                                      amount >= min_raise);
                   }) != actions.cend());
}

auto execute_action(enums::action action, uint32_t amount, player &player, uint32_t min_raise, uint32_t highest_bet)
                -> uint32_t {
  uint32_t new_min_raise = min_raise;

  switch (action) {
  case enums::action::fold: {
    player.state = enums::state::out;
  } break;
  case enums::action::check: {
    player.state = enums::state::alive;
  } break;
  case enums::action::call:
  case enums::action::bet:
  case enums::action::raise: {
    uint32_t raise_size = amount + player.bet - highest_bet;
    if (raise_size > min_raise) {
      new_min_raise = raise_size;
    }

    player.stack -= amount;
    player.bet += amount;
    player.round_bet += amount;

    player.state = !player.stack ? enums::state::allin : enums::state::alive;
  } break;
  default: {
    throw exceptions::actions_error{ "Got invalid action to execute" };
  }
  }

  return new_min_raise;
}
} // namespace pokerengine
#endif // POKERENGINE_ACTION_HPP
