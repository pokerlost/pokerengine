//
// Created by copper_boy on 10/6/23.
//

#ifndef POKERENGINE_ENGINE_HPP
#define POKERENGINE_ENGINE_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <compare>
#include <cstdint>
#include <functional>
#include <map>
#include <numeric>
#include <span>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <magic_enum/magic_enum.hpp>

#include "card/cards.hpp"
#include "constants.hpp"
#include "enums.hpp"
#include "evaluator/evaluation_result.hpp"
#include "evaluator/result.hpp"
#include "pokerengine.hpp"
#include "vector.hpp"

namespace pokerengine {
struct player {
  bool is_left = false;

  int32_t stack;
  int32_t behind;
  int32_t front;
  int32_t round_bet;

  enums::state state;

  std::string id;

  auto operator<=>(const player &other) const noexcept -> std::strong_ordering = delete;
  auto operator==(const player &other) const noexcept -> bool = default;

  explicit operator std::string() const {
    using namespace std::literals;

    return "Player<is_left="s + std::to_string(is_left) + ", stack="s + std::to_string(stack) +
                    ", behind="s + std::to_string(behind) + ", front="s + std::to_string(front) +
                    ", round_bet="s + std::to_string(round_bet) + ", state="s +
                    std::string{ magic_enum::enum_name(state) } + ", id="s + id + ">"s;
  }
};


struct player_action {
  int32_t amount;
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

namespace v1 {
template < typename Engine >
class engine_detail {
  public:
  explicit engine_detail(Engine &engine) : engine{ engine } {
  }

  protected:
  Engine &engine;
};

auto is_no_actions_available(enums::round round, enums::state state) noexcept -> bool {
  return (state == enums::state::out || state == enums::state::allin) || round == enums::round::none ||
                  round == enums::round::showdown;
}

auto get_fold_or_check(enums::position player, int32_t highest_round_bet, int32_t committed) noexcept
                -> player_action {
  if (committed == highest_round_bet || !highest_round_bet) {
    return player_action{ 0, enums::action::check, player };
  }
  return player_action{ 0, enums::action::fold, player };
}

auto is_bet_available(int32_t bb_bet, int32_t highest_round_bet, int32_t remaining) noexcept -> bool {
  return !highest_round_bet && remaining > bb_bet;
}

auto is_call_available(int32_t highest_round_bet, int32_t committed, int32_t remaining) noexcept -> bool {
  if (committed + remaining <= highest_round_bet) {
    return true;
  }

  return highest_round_bet && (committed < highest_round_bet && (committed + remaining) >= highest_round_bet);
}

auto is_raise_available(
                enums::state state,
                int32_t highest_round_bet,
                int32_t min_raise,
                int32_t committed,
                int32_t remaining) -> bool {
  if ((committed + remaining) <= highest_round_bet) {
    return false;
  }

  return (state == enums::state::init || state == enums::state::alive) ||
                  (committed < highest_round_bet && (highest_round_bet - committed >= min_raise));
}

auto get_possible_actions(
                enums::round round,
                enums::position player,
                enums::state state,
                bool is_left,
                int32_t bb_bet,
                int32_t min_raise,
                int32_t highest_round_bet,
                int32_t committed,
                int32_t remaining) -> std::vector< player_action > {
  if (is_no_actions_available(round, state)) {
    return std::vector< player_action >{ player_action{ 0, enums::action::none, enums::position::none } };
  }

  std::vector< player_action > actions{ get_fold_or_check(player, highest_round_bet, committed) };

  if (is_left) {
    return actions;
  }

  if (is_bet_available(bb_bet, highest_round_bet, remaining)) {
    actions.emplace_back(remaining, enums::action::bet, player);
  } else {
    if (is_raise_available(state, highest_round_bet, min_raise, committed, remaining)) {
      actions.emplace_back(remaining, enums::action::raise, player);
    }
  }

  if (is_call_available(highest_round_bet, committed, remaining)) {
    actions.emplace_back(highest_round_bet - committed, enums::action::call, player);
  }

  return actions;
}

auto is_normal_action(
                const player_action &pa,
                const std::vector< player_action > &all_actions,
                enums::position player,
                int32_t min_raise) -> bool {
  return pa.position == player && player != enums::position::none &&
                  (std::find_if(all_actions.cbegin(), all_actions.cend(), [&](const auto &element) -> bool {
                     return (element == pa) ||
                                     (pa.action == enums::action::bet && element.action == enums::action::bet &&
                                      pa.amount < element.amount && pa.amount >= min_raise) ||
                                     (pa.action == enums::action::raise &&
                                      element.action == enums::action::raise && pa.amount < element.amount &&
                                      pa.amount >= min_raise);
                   }) != all_actions.cend());
}

auto execute_action(const player_action &pa, player &player, int32_t min_raise, int32_t highest_round_bet)
                -> int32_t {
  int32_t new_min_raise = min_raise;

  switch (pa.action) {
  case enums::action::fold: {
    player.state = enums::state::out;
  } break;
  case enums::action::check: {
    player.state = enums::state::alive;
  } break;
  case enums::action::call:
  case enums::action::bet:
  case enums::action::raise: {
    int32_t raise_size = pa.amount + player.round_bet - highest_round_bet;
    if (raise_size > min_raise) {
      new_min_raise = raise_size;
    }

    player.behind -= pa.amount;
    player.front += pa.amount;
    player.round_bet += pa.amount;

    player.state = player.behind == 0 ? enums::state::allin : enums::state::alive;
  } break;
  default: {
    throw std::runtime_error{ "Got invalid action to execute" };
  }
  }

  return new_min_raise;
}

auto get_players(const std::vector< player > &players, bool rotate = false) -> std::vector< player > {
  std::vector< player > result;
  std::copy_if(players.cbegin(), players.cend(), std::back_inserter(result), [](const auto &player) -> bool {
    return !(player.stack <= 0 || player.is_left);
  });

  if (result.size() < constants::MIN_PLAYERS || result.size() > constants::MAX_PLAYERS) {
    throw std::runtime_error{ "Players size invalid" };
  }

  if (rotate) {
    std::rotate(result.rbegin(), result.rbegin() + 1, result.rend());
  }

  return result;
}

auto is_all_allin(const std::vector< player > &players) -> bool {
  if (players.size() != constants::MIN_PLAYERS) {
    return false;
  }

  return players[static_cast< uint8_t >(enums::position::sb)].state == enums::state::allin &&
                  players[static_cast< uint8_t >(enums::position::bb)].state == enums::state::allin;
}

auto set_blinds(std::vector< player > &players, int32_t sb_bet, int32_t bb_bet) -> void {
  std::for_each(players.begin(), players.end(), [&, index = 0](auto &player) mutable -> void {
    player.state = enums::state::init;
    player.behind = player.stack;
    player.round_bet = 0;
    player.front = 0;

    if (index < 2) {
      player.state = index == 0 ? enums::state::alive : enums::state::init;
      player.front = index == 0 ? sb_bet : bb_bet;

      if (player.front > player.behind) {
        player.front = player.behind;
      }

      player.behind -= player.front;
      player.round_bet = player.front;

      if (player.behind == 0) {
        player.state = enums::state::allin;
      }
    }

    index++;
  });
}

auto get_chips_to_return(const std::vector< player > &players, int32_t highest_bet)
                -> std::pair< enums::position, int32_t > {
  if (std::count_if(players.cbegin(), players.cend(), [&](const auto &element) -> bool {
        return element.front == highest_bet;
      }) < 2) {
    std::vector< int32_t > chips_front;
    std::for_each(players.cbegin(), players.cend(), [&](const auto &element) -> void {
      chips_front.push_back(element.front);
    });


    auto position = std::distance(
                    players.cbegin(),
                    std::find_if(players.cbegin(), players.cend(), [&](const auto &element) -> bool {
                      return element.front == highest_bet;
                    }));

    std::sort(chips_front.begin(), chips_front.end(), std::greater{});
    return std::make_pair(enums::position(position), highest_bet - chips_front[1]);
  } else {
    return std::make_pair(enums::position{ 0 }, 0);
  }
}

auto get_chips_front(const std::vector< player > &players, int32_t highest_bet) -> std::vector< int32_t > {
  std::vector< int32_t > chips;
  for (const auto &player : players) {
    chips.push_back(player.front);
  }

  auto chips_return = get_chips_to_return(players, highest_bet);
  if (chips_return.second == 0) {
    return chips;
  }
  chips[static_cast< uint8_t >(chips_return.first)] -= chips_return.second;
  return chips;
}

auto get_all_pots(const std::vector< player > &players, int32_t highest_bet)
                -> std::vector< std::tuple< std::vector< uint8_t >, int32_t, int32_t > > {
  auto chips_front = get_chips_front(players, highest_bet);

  std::vector< std::pair< int32_t, uint8_t > > chips_and_players;
  for (size_t i = 0; i < chips_front.size(); i++) {
    chips_and_players.emplace_back(chips_front[i], i);
  }
  std::sort(chips_and_players.begin(), chips_and_players.end(), [](const auto &lhs, const auto &rhs) -> bool {
    return lhs.first > rhs.first;
  });

  std::vector< uint8_t > main_pot_players;
  std::vector< std::tuple< std::vector< uint8_t >, int32_t, int32_t > > pots;

  int32_t upper = chips_and_players[0].first;
  std::for_each(chips_and_players.cbegin(), chips_and_players.cend(), [&](const auto &pair) -> void {
    if (players[pair.second].state == enums::state::out) {
      return;
    } else if (int32_t lower = chips_front[pair.second]; lower == upper) {
      main_pot_players.push_back(pair.second);
    } else if (players[pair.second].state == enums::state::allin) {
      pots.emplace_back(main_pot_players, upper, lower);
      upper = lower;
      main_pot_players.push_back(pair.second);
    }
  });
  pots.emplace_back(main_pot_players, upper, 0);

  return pots;
}

template < uint8_t A = 0, uint8_t B = 1 >
  requires(A >= 0 && B > 0 && A < B)
auto get_adjust_pot(const std::vector< player > &players, int32_t highest_bet, bool flop_dealt) -> int32_t {
  std::vector< int32_t > chips_front;
  std::for_each(players.cbegin(), players.cend(), [&](auto const &element) {
    chips_front.push_back(element.front);
  });

  auto pot = std::reduce(chips_front.cbegin(), chips_front.cend());
  if (flop_dealt && constants::RAKE< A, B > != 0.0) {
    auto chips_returned = get_chips_to_return(players, highest_bet).second;
    return static_cast< int32_t >((pot - chips_returned) * constants::RAKE< A, B > + chips_returned);
  } else {
    return pot;
  }
}

auto adjust_side_pot(const std::vector< player > &players, int32_t upper_bound, int32_t lower_bound) noexcept
                -> std::vector< int32_t > {
  std::vector< int32_t > result;
  for (auto const &player : players) {
    auto chips = player.front;
    result.push_back(
                    chips <= lower_bound                ? 0 :
                                    chips > upper_bound ? upper_bound - lower_bound :
                                                          chips - lower_bound);
  }

  return result;
}

template < uint8_t A = 0, uint8_t B = 1 >
  requires(A >= 0 && B > 0 && A < B)
auto get_side_pot_redistribution(
                const std::vector< player > &ps,
                const cards &cards,
                const std::vector< uint8_t > &players,
                bool flop_dealt,
                int32_t upper_bound,
                int32_t lower_bound) -> std::vector< int32_t > {
  auto winners = get_evaluation_result(cards, players);
  auto chips_adjusted = adjust_side_pot(ps, upper_bound, lower_bound);

  auto total_pot = static_cast< int32_t >(
                  std::accumulate(chips_adjusted.cbegin(), chips_adjusted.cend(), 0) *
                  (flop_dealt ? constants::RAKE_MULTI< A, B > : 1.0f));
  int32_t amount_each_winner = total_pot / static_cast< int32_t >(winners.size());

  std::vector< int32_t > result;
  for (size_t index = 0; index < ps.size(); index++) {
    auto winner = std::find_if(winners.cbegin(), winners.cend(), [&](const auto &element) {
      return element.second == index;
    });
    if (winner != winners.cend()) {
      result.push_back(-chips_adjusted[index] + amount_each_winner);
    } else {
      result.push_back(-chips_adjusted[index]);
    }
  }

  return result;
}

auto get_next_round(enums::round round) -> std::tuple< enums::round, bool > {
  auto result = static_cast< enums::round >(static_cast< int8_t >(round) + 1);
  return std::make_tuple(result, result == enums::round::flop);
}
} // namespace v1

template < typename Engine >
class players_manager : public actual::engine_detail< Engine > {
  public:
  using actual::engine_detail< Engine >::engine_detail;

  [[nodiscard]] auto get_players() const noexcept -> std::vector< player > {
    return players_;
  }

  [[nodiscard]] auto get_player(int8_t index) -> player & {
    return (index < 0 || index > players_.size()) ? default_player_ : *(players_.begin() + index);
  }

  auto set_players(const std::vector< player > &players) noexcept -> void {
    players_ = players;
  }

  auto add_player(int32_t stack, const std::string &id) -> void {
    for (const auto &player : players_) {
      if (player.id == id) {
        throw std::runtime_error{ "Player already in the game" };
      }
    }

    auto traits = this->engine.get_engine_traits();
    if (stack < traits.get_bb_bet() * traits.get_bb_mult()) {
      throw std::runtime_error{ "Player stack less than game minimal stacksize" };
    }

    players_.emplace_back(false, stack, 0, 0, 0, enums::state::init, id);
  }

  auto remove_player(const std::string &id) -> void {
    for (auto &player : players_) {
      if (player.id == id) {
        player.is_left = true;
      }
    }
  }


  private:
  std::vector< player > players_;
  player default_player_ = player(true, 0, 0, 0, 0, enums::state::none, "DEFAULT");
};

template < typename Engine >
class actions_manager : public actual::engine_detail< Engine > {
  public:
  using actual::engine_detail< Engine >::engine_detail;

  [[nodiscard]] auto get_possible_actions() const -> std::vector< player_action > {
    auto player = this->engine.positions.get_player();
    return actual::get_possible_actions(
                    this->engine.round.get_round(),
                    this->engine.positions.get_current(),
                    player.state,
                    player.is_left,
                    this->engine.get_engine_traits().get_bb_bet(),
                    this->engine.get_engine_traits().get_min_raise(),
                    this->engine.pot.get_highest_bet(),
                    player.round_bet,
                    player.behind);
  }

  auto execute(const player_action &pa) -> void {
    if (!actual::is_normal_action(
                        pa,
                        get_possible_actions(),
                        this->engine.positions.get_current(),
                        this->engine.get_engine_traits().get_min_raise())) {
      throw std::runtime_error{ "Execute action failed" };
    }

    auto &player = this->engine.positions.get_player();
    this->engine.get_engine_traits().set_min_raise(actual::execute_action(
                    pa,
                    player,
                    this->engine.get_engine_traits().get_min_raise(),
                    this->engine.pot.get_highest_bet()));

    auto iterable = this->engine.players.get_players();
    if (auto actions = this->engine.positions.get_actionable();
        this->engine.positions.get_number_alive() < constants::MIN_PLAYERS ||
        (actions == 0 && this->engine.positions.get_future_actionable() < constants::MIN_PLAYERS)) {
      if (!this->engine.round.get_flop_dealt()) {
        this->engine.round.set_flop_dealt(
                        std::find_if(iterable.cbegin(), iterable.cend(), [](const auto &value) -> bool {
                          return value.state == enums::state::allin;
                        }) != iterable.cend());
      }

      this->engine.round.set_round(enums::round::showdown);
    } else if (auto last_player = std::distance(
                               iterable.cbegin(),
                               std::find_if(iterable.cbegin(),
                                            iterable.cend(),
                                            [](const auto &element) -> bool {
                                              return element.state != enums::state::out &&
                                                              element.state != enums::state::allin;
                                            }));
               this->engine.positions.get_actionable() == 1 &&
               this->engine.positions.get_future_actionable() == 1 &&
               iterable[last_player].round_bet == this->engine.pot.get_highest_bet()) {
      this->engine.round.set_flop_dealt(true), this->engine.round.set_round(enums::round::showdown);
    } else if (actions == 0) {
      if (this->engine.round.get_round() == enums::round::river) {
        this->engine.round.set_round(enums::round::showdown);
      } else {
        this->engine.positions.set_next_round_player();

        this->engine.get_engine_traits().set_min_raise(this->engine.get_engine_traits().get_bb_bet());

        auto next_round = actual::get_next_round(this->engine.round.get_round());
        this->engine.round.set_round(std::get< 0 >(next_round)),
                        this->engine.round.set_flop_dealt(std::get< 1 >(next_round));

        std::for_each(iterable.begin(), iterable.end(), [](auto &element) {
          element.round_bet = 0;
          element.state = element.state == enums::state::alive ? enums::state::init : element.state;
        });
      }
    } else {
      this->engine.positions.set_next_current();
    }

    this->engine.players.set_players(iterable);
  }
};

template < typename Engine >
class positions_manager : public actual::engine_detail< Engine > {
  public:
  using actual::engine_detail< Engine >::engine_detail;

  [[nodiscard]] auto get_current() const noexcept -> enums::position {
    return current_;
  }

  [[nodiscard]] auto get_player() const -> player {
    return this->engine.players.get_player(static_cast< int8_t >(get_current()));
  }

  [[nodiscard]] auto get_player() -> player & {
    return this->engine.players.get_player(static_cast< int8_t >(get_current()));
  }

  [[nodiscard]] auto get_number_alive() const -> int {
    auto iterable = this->engine.players.get_players();
    return std::accumulate(iterable.cbegin(), iterable.cend(), 0, [&](int value, auto const &element) -> int {
      return element.state != enums::state::out ? value + 1 : value;
    });
  }

  [[nodiscard]] auto get_actionable() const -> int {
    auto iterable = this->engine.players.get_players();
    return std::accumulate(iterable.cbegin(), iterable.cend(), 0, [&](int value, auto const &element) -> int {
      return (element.state == enums::state::init ||
              (element.state == enums::state::alive && element.round_bet < this->engine.pot.get_highest_bet())) ?
                      value + 1 :
                      value;
    });
  }

  [[nodiscard]] auto get_future_actionable() const -> int {
    auto iterable = this->engine.players.get_players();
    return std::accumulate(iterable.cbegin(), iterable.cend(), 0, [&](int value, auto const &element) -> int {
      return element.state != enums::state::out && element.state != enums::state::allin ? value + 1 : value;
    });
  }

  [[nodiscard]] auto is_showdown() const -> bool {
    return get_number_alive() > 1;
  }

  auto set_current(enums::position position) -> void {
    current_ = position;
  }

  auto set_next_current() -> void {
    auto iterable_size = this->engine.players.get_players().size();
    do {
      set_current(static_cast< enums::position >((static_cast< int8_t >(get_current()) + 1) % iterable_size));
    } while (get_player().state == enums::state::out || get_player().state == enums::state::allin);
  }

  auto set_next_round_player() -> void {
    set_current(enums::position::sb);

    auto iterable_size = this->engine.players.get_players().size();
    while (get_player().state == enums::state::out || get_player().state == enums::state::allin) {
      set_current(static_cast< enums::position >((static_cast< int8_t >(get_current()) + 1) % iterable_size));
    }
  }

  private:
  enums::position current_{ enums::position::none };
};

template < typename Engine, uint8_t A = 0, uint8_t B = 1 >
  requires(A >= 0 && B > 0 && A < B)
class pot_manager : public actual::engine_detail< Engine > {
  public:
  using actual::engine_detail< Engine >::engine_detail;

  [[nodiscard]] auto get_highest_game_bet() const -> int32_t {
    auto iterable = this->engine.players.get_players();
    auto max = std::max_element(
                    iterable.cbegin(), iterable.cend(), [](const auto &lhs, const auto &rhs) -> bool {
                      return lhs.front < rhs.front;
                    });

    return max == iterable.end() ? 0 : max->front;
  }

  [[nodiscard]] auto get_highest_bet() const -> int32_t {
    auto iterable = this->engine.players.get_players();
    auto max = std::max_element(
                    iterable.cbegin(), iterable.cend(), [](const auto &lhs, const auto &rhs) -> bool {
                      return lhs.round_bet < rhs.round_bet;
                    });

    return max == iterable.end() ? 0 : max->round_bet;
  }

  [[nodiscard]] auto pot() const noexcept -> int32_t {
    auto iterable = this->engine.players.get_players();
    std::vector< int32_t > chips_front;
    for (auto const &player : iterable) {
      chips_front.push_back(player.front);
    }

    auto reduce = std::reduce(chips_front.cbegin(), chips_front.cend());
    return this->engine.round.get_flop_dealt() ?
                    reduce - actual::get_chips_to_return(iterable, get_highest_bet()).second :
                    reduce;
  }

  [[nodiscard]] auto pot_rake() noexcept -> int32_t {
    auto flop_dealt = this->engine.round.get_flop_dealt();
    return flop_dealt ? static_cast< int32_t >(pot() * constants::RAKE_MULTI< A, B >) : pot();
  }

  auto pay(const cards &cards) -> std::vector< std::pair< result, int32_t > > {
    auto iterable = this->engine.players.get_players();
    auto pots = actual::get_all_pots(iterable, get_highest_game_bet());
    auto chips = std::accumulate(
                    pots.cbegin(), pots.cend(), std::vector< int32_t >{}, [&](auto value, const auto &element) {
                      return value +
                                      actual::get_side_pot_redistribution(
                                                      iterable,
                                                      cards,
                                                      std::get< 0 >(element),
                                                      this->engine.round.get_flop_dealt(),
                                                      std::get< 1 >(element),
                                                      std::get< 2 >(element));
                    });

    std::vector< std::pair< result, int32_t > > results;
    std::for_each(iterable.begin(), iterable.end(), [&, index = 0](auto &player) mutable -> void {
      auto result = results.emplace_back(get_evaluation_result_one(cards, index), chips[index++]);
      player.stack += result.second;
    });
    this->engine.players.set_players(iterable);

    return results;
  }

  auto pay_noshowdown() -> std::vector< int32_t > {
    auto iterable = this->engine.players.get_players();
    auto adjusted_pot = actual::get_adjust_pot< A, B >(
                    iterable, get_highest_game_bet(), this->engine.round.get_flop_dealt());
    auto winner = std::distance(
                    iterable.cbegin(),
                    std::find_if(iterable.cbegin(), iterable.cend(), [](const auto &element) {
                      return element.state != enums::state::out;
                    }));

    std::vector< int32_t > results;
    std::for_each(iterable.begin(), iterable.end(), [&, index = 0](auto &player) mutable -> void {
      auto result = results.emplace_back(index++ == winner ? -player.front + adjusted_pot : -player.front);
      player.stack += result;
    });
    this->engine.players.set_players(iterable);

    return results;
  }
};

template < typename Engine >
class round_manager : public actual::engine_detail< Engine > {
  public:
  using actual::engine_detail< Engine >::engine_detail;

  auto reset() noexcept -> void {
    set_round(enums::round::preflop);
    set_flop_dealt(false);
  }

  [[nodiscard]] auto in_terminal_state() const noexcept -> bool {
    return get_round() == enums::round::showdown;
  }

  [[nodiscard]] auto get_round() const noexcept -> enums::round {
    return round_;
  }

  [[nodiscard]] auto get_flop_dealt() const noexcept -> bool {
    return flop_dealt_;
  }

  auto set_round(enums::round value) noexcept -> void {
    round_ = value;
  }

  auto set_flop_dealt(bool value) noexcept -> void {
    flop_dealt_ = value;
  }

  private:
  enums::round round_{ enums::round::none };
  bool flop_dealt_{ false };
};

class engine_traits {
  public:
  engine_traits() = delete;

  engine_traits(int32_t sb_bet, int32_t bb_bet, uint8_t bb_mult, int32_t min_raise = -1)
          : sb_bet_{ sb_bet }, bb_bet_{ bb_bet }, bb_mult_{ bb_mult } {
    min_raise_ = min_raise > 0 ? min_raise : bb_bet_ * 2;
  }

  [[nodiscard]] auto get_sb_bet() const noexcept -> int32_t {
    return sb_bet_;
  }

  [[nodiscard]] auto get_bb_bet() const noexcept -> int32_t {
    return bb_bet_;
  }

  [[nodiscard]] auto get_bb_mult() const noexcept -> uint8_t {
    return bb_mult_;
  }

  [[nodiscard]] auto get_min_raise() const noexcept -> int32_t {
    return min_raise_;
  }

  auto set_sb_bet(int32_t value) noexcept -> void {
    sb_bet_ = value;
  }

  auto set_bb_bet(int32_t value) noexcept -> void {
    bb_bet_ = value;
  }

  auto set_bb_mult(uint8_t value) noexcept -> void {
    bb_mult_ = value;
  }

  auto set_min_raise(int32_t value) noexcept -> void {
    min_raise_ = value;
  }

  private:
  int32_t sb_bet_;
  int32_t bb_bet_;
  uint8_t bb_mult_;

  int32_t min_raise_;
};

template < uint8_t A = 0, uint8_t B = 1 >
  requires(A >= 0 && B > 0 && A < B)
class engine {
  public:
  engine() = delete;

  explicit engine(const engine_traits &engine_traits) : engine_traits_{ engine_traits } {
  }

  auto load(const engine_traits &engine_traits,
            const std::vector< player > &ps,
            enums::position position,
            enums::round rd,
            bool flop_dealt) -> void {
    set_engine_traits(engine_traits);
    players.set_players(ps);
    positions.set_current(position);
    round.set_round(rd);
    round.set_flop_dealt(flop_dealt);
  }

  auto start(bool is_new_game = false) -> void {
    stop();

    auto new_players = actual::get_players(players.get_players(), is_new_game);
    actual::set_blinds(new_players, engine_traits_.get_sb_bet(), engine_traits_.get_bb_bet());

    players.set_players(new_players);

    if (actual::is_all_allin(new_players)) {
      round.set_round(enums::round::showdown);
    } else {
      round.set_round(enums::round::preflop);

      positions.set_current(
                      new_players.size() > constants::MIN_PLAYERS ? enums::position::utg :
                                                                    enums::position::sb);

      if (positions.get_player().state == enums::state::allin) {
        positions.set_next_current();
      }
    }
  }

  auto stop() -> void {
    engine_traits_.set_min_raise(engine_traits_.get_bb_bet());
    round.reset();
  }

  [[nodiscard]] auto get_engine_traits() noexcept -> engine_traits & {
    return engine_traits_;
  }

  auto set_engine_traits(const engine_traits &engine_traits) noexcept -> void {
    engine_traits_ = engine_traits;
  }

  actions_manager< engine< A, B > > actions = actions_manager< engine< A, B > >{ *this };
  players_manager< engine< A, B > > players = players_manager< engine< A, B > >{ *this };
  positions_manager< engine< A, B > > positions = positions_manager< engine< A, B > >{ *this };
  pot_manager< engine< A, B >, A, B > pot = pot_manager< engine< A, B > >{ *this };
  round_manager< engine< A, B > > round = round_manager< engine< A, B > >{ *this };

  private:
  engine_traits engine_traits_;
};
} // namespace pokerengine

#endif // POKERENGINE_ENGINE_HPP
