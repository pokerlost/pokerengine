#ifndef POKERENGINE_ENGINE_HPP
#define POKERENGINE_ENGINE_HPP

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <stdexcept>

#include "action.hpp"
#include "enums.hpp"
#include "player.hpp"
#include "pokerengine.hpp"
#include "pot.hpp"
#include "round.hpp"
#include "vector.hpp"

namespace pokerengine {
namespace exceptions {
class engine_error : public exception {
  public:
  using exception::exception;

  protected:
  std::string class_name_ = "engine_error";
};
} // namespace exceptions

class engine_traits {
  public:
  engine_traits(uint16_t sb_bet, uint16_t bb_bet, uint8_t bb_mult, uint32_t min_raise = 0)
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

  engine(const engine_traits &engine_traits,
         enums::position current,
         enums::round round,
         bool flop_dealt,
         const std::vector< player > &players)
          : engine(engine_traits) {
    set_current(current);
    set_round(round);
    set_flop_dealt(flop_dealt);
    set_players(players);
  }

  explicit engine(const engine_traits &engine_traits) : engine_traits_{ engine_traits } {
  }

  auto start(bool is_new_game = false) -> void {
    stop();

    auto players = get_players();
    std::vector< player > new_players;
    std::copy_if(players.cbegin(),
                 players.cend(),
                 std::back_inserter(new_players),
                 [](const auto &player) -> bool { return player.stack > 0; });

    if (new_players.size() < constants::MIN_PLAYERS || new_players.size() > constants::MAX_PLAYERS) {
      throw exceptions::engine_error{ "Players size invalid" };
    }

    if (is_new_game) {
      std::rotate(new_players.rbegin(), new_players.rbegin() + 1, new_players.rend());
    }

    set_blinds(new_players, get_engine_traits().get_sb_bet(), get_engine_traits().get_bb_bet());
    set_players(new_players);

    if (new_players.size() == constants::MIN_PLAYERS &&
        new_players[static_cast< uint8_t >(enums::position::sb)].state == enums::state::allin &&
        new_players[static_cast< uint8_t >(enums::position::bb)].state == enums::state::allin) {
      set_round(enums::round::showdown);
    } else {
      set_round(enums::round::preflop);

      set_current(new_players.size() > constants::MIN_PLAYERS ? enums::position::utg : enums::position::sb);

      if (get_current_player().state == enums::state::allin) {
        set_next_current();
      }
    }
  }

  auto stop() -> void {
    get_engine_traits().set_min_raise(get_engine_traits().get_bb_bet());
    reset();
  }

  [[nodiscard]] auto get_current() noexcept -> enums::position {
    return current_;
  }

  [[nodiscard]] auto get_current_player() noexcept -> player & {
    return get_player(static_cast< uint8_t >(get_current()));
  }

  [[nodiscard]] auto get_engine_traits() noexcept -> engine_traits & {
    return engine_traits_;
  }

  [[nodiscard]] auto get_flop_dealt() noexcept -> bool {
    return flop_dealt_;
  }

  [[nodiscard]] auto get_highest_bet() -> uint32_t {
    auto iterable = get_players();
    auto max = std::max_element(
                    iterable.cbegin(), iterable.cend(), [](const auto &lhs, const auto &rhs) -> bool {
                      return lhs.round_bet < rhs.round_bet;
                    });

    return max == iterable.end() ? 0 : max->round_bet;
  }

  [[nodiscard]] auto get_highest_game_bet() -> uint32_t {
    auto iterable = get_players();
    auto max = std::max_element(
                    iterable.cbegin(), iterable.cend(), [](const auto &lhs, const auto &rhs) -> bool {
                      return lhs.bet < rhs.bet;
                    });

    return max == iterable.end() ? 0 : max->bet;
  }

  [[nodiscard]] auto get_players() noexcept -> std::vector< player > & {
    return players_;
  }

  [[nodiscard]] auto get_possible_actions() -> std::vector< player_action > {
    auto player = get_current_player();
    return ::pokerengine::get_possible_actions(
                    get_round(),
                    get_current(),
                    player.state,
                    get_engine_traits().get_bb_bet(),
                    get_engine_traits().get_min_raise(),
                    get_highest_bet(),
                    player.round_bet,
                    player.stack);
  }

  [[nodiscard]] auto get_round() noexcept -> enums::round {
    return round_;
  }

  [[nodiscard]] auto get_pot() noexcept -> uint32_t {
    return get_flop_dealt() ? static_cast< uint32_t >(get_default_pot() * constants::RAKE_MULTI< A, B >) :
                              get_default_pot();
  }

  auto set_engine_traits(const engine_traits &engine_traits) noexcept -> void {
    engine_traits_ = engine_traits;
  }

  auto join_player(uint32_t stack, const std::string &id) -> void {
    for (const auto &player : get_players()) {
      if (player.id == id) {
        throw exceptions::engine_error{ "Player already in the game" };
      }
    }
    if (stack < engine_traits_.get_bb_bet() * engine_traits_.get_bb_mult()) {
      throw exceptions::engine_error{ "Player stack less than game minimal stacksize" };
    }

    add_player(player{ .stack = stack, .bet = 0, .round_bet = 0, .state = enums::state::init, .id = id });
  }

  [[nodiscard]] auto in_terminal_state() -> bool {
    return get_number_alive() > 1;
  }

  [[nodiscard]] auto is_showdown() noexcept -> bool {
    return get_round() == enums::round::showdown;
  }

  auto pay(const cards &cards) -> std::vector< std::pair< result, int32_t > > {
    auto iterable = get_players();
    auto pots = get_all_pots(iterable, get_highest_game_bet());
    auto chips = std::accumulate(
                    pots.cbegin(), pots.cend(), std::vector< int32_t >{}, [&](auto value, const auto &element) {
                      return value +
                                      get_side_pot_redistribution(
                                                      iterable,
                                                      cards,
                                                      std::get< 0 >(element),
                                                      get_flop_dealt(),
                                                      std::get< 1 >(element),
                                                      std::get< 2 >(element));
                    });

    std::vector< std::pair< result, int32_t > > results;
    std::for_each(iterable.begin(), iterable.end(), [&, index = 0](auto &player) mutable -> void {
      auto result = results.emplace_back(get_evaluation_result_one(cards, index), chips[index++]);
      player.stack += result.second;
    });
    set_players(iterable);

    return results;
  }

  auto pay_noshowdown() -> std::vector< int32_t > {
    auto iterable = get_players();
    auto adjusted_pot = get_adjust_pot< A, B >(iterable, get_highest_game_bet(), get_flop_dealt());
    auto winner = std::distance(
                    iterable.cbegin(),
                    std::find_if(iterable.cbegin(), iterable.cend(), [](const auto &element) {
                      return element.state != enums::state::out;
                    }));

    std::vector< int32_t > results;
    std::for_each(iterable.begin(), iterable.end(), [&, index = 0](auto &player) mutable -> void {
      auto result = results.emplace_back(index++ == winner ? -player.bet + adjusted_pot : -player.bet);
      player.stack += result;
    });
    set_players(iterable);

    return results;
  }

  auto execute(const player_action &player_action) -> void {
    if (!is_action_allowed(
                        get_possible_actions(),
                        get_current(),
                        player_action.position,
                        player_action.action,
                        player_action.amount,
                        get_engine_traits().get_min_raise())) {
      throw exceptions::engine_error{ "Execute action failed, not normal action" };
    }

    auto &player = get_current_player();
    get_engine_traits().set_min_raise(
                    execute_action(player_action.action,
                                   player_action.amount,
                                   player,
                                   get_engine_traits().get_min_raise(),
                                   get_highest_bet()));

    auto iterable = get_players();
    if (auto actions = get_actionable(); get_number_alive() < constants::MIN_PLAYERS ||
        (actions == 0 && get_future_actionable() < constants::MIN_PLAYERS)) {
      if (!get_flop_dealt()) {
        set_flop_dealt(std::find_if(iterable.cbegin(), iterable.cend(), [](const auto &value) -> bool {
                         return value.state == enums::state::allin;
                       }) != iterable.cend());
      }

      set_round(enums::round::showdown);
    } else if (auto last_player = std::distance(
                               iterable.cbegin(),
                               std::find_if(iterable.cbegin(),
                                            iterable.cend(),
                                            [](const auto &element) -> bool {
                                              return element.state != enums::state::out &&
                                                              element.state != enums::state::allin;
                                            }));
               get_actionable() == 1 && get_future_actionable() == 1 &&
               iterable[last_player].round_bet == get_highest_bet()) {
      set_flop_dealt(true);
      set_round(enums::round::showdown);
    } else if (actions == 0) {
      if (get_round() == enums::round::river) {
        set_round(enums::round::showdown);
      } else {
        set_next_round_player();
        get_engine_traits().set_min_raise(get_engine_traits().get_bb_bet());

        auto next_round = get_next_round(get_round());
        set_round(next_round);
        set_flop_dealt(next_round >= enums::round::flop);

        std::for_each(iterable.begin(), iterable.end(), [](auto &element) {
          element.round_bet = 0;
          element.state = element.state == enums::state::alive ? enums::state::init : element.state;
        });
      }
    } else {
      set_next_current();
    }

    set_players(iterable);
  }

  protected:
  [[nodiscard]] auto get_actionable() -> int {
    auto iterable = get_players();
    return std::accumulate(iterable.cbegin(), iterable.cend(), 0, [&](int value, auto const &element) -> int {
      return (element.state == enums::state::init ||
              (element.state == enums::state::alive && element.round_bet < get_highest_bet())) ?
                      value + 1 :
                      value;
    });
  }

  [[nodiscard]] auto get_future_actionable() -> int {
    auto iterable = get_players();
    return std::accumulate(iterable.cbegin(), iterable.cend(), 0, [&](int value, auto const &element) -> int {
      return element.state != enums::state::out && element.state != enums::state::allin ? value + 1 : value;
    });
  }

  [[nodiscard]] auto get_number_alive() -> int {
    auto iterable = get_players();
    return std::accumulate(iterable.cbegin(), iterable.cend(), 0, [&](int value, auto const &element) -> int {
      return element.state != enums::state::out ? value + 1 : value;
    });
  }

  private:
  auto get_player(uint8_t index) -> player & {
    if (index < 0 || index > get_players().size()) {
      throw exceptions::engine_error{ "Invalid index" };
    }

    return *(get_players().begin() + index);
  }

  [[nodiscard]] auto get_default_pot() noexcept -> uint32_t {
    auto iterable = get_players();
    std::vector< uint32_t > chips_bet;
    for (auto const &player : iterable) {
      chips_bet.push_back(player.bet);
    }

    auto reduce = std::reduce(chips_bet.cbegin(), chips_bet.cend());
    return get_flop_dealt() ? reduce - get_chips_to_return(iterable, get_highest_bet()).second : reduce;
  }

  auto set_current(enums::position position) -> void {
    current_ = position;
  }

  auto set_flop_dealt(bool value) noexcept -> void {
    flop_dealt_ = value;
  }

  auto set_next_current() -> void {
    auto iterable_size = get_players().size();
    do {
      set_current(static_cast< enums::position >((static_cast< uint8_t >(get_current()) + 1) % iterable_size));
    } while (get_current_player().state == enums::state::out ||
             get_current_player().state == enums::state::allin);
  }

  auto set_next_round_player() -> void {
    set_current(enums::position::sb);

    auto iterable_size = get_players().size();
    while (get_current_player().state == enums::state::out ||
           get_current_player().state == enums::state::allin) {
      set_current(static_cast< enums::position >((static_cast< uint8_t >(get_current()) + 1) % iterable_size));
    }
  }

  auto set_players(const std::vector< player > &players) noexcept -> void {
    players_ = players;
  }


  auto set_round(enums::round value) noexcept -> void {
    round_ = value;
  }

  auto add_player(const player &player) -> void {
    players_.push_back(player);
  }


  auto reset() noexcept -> void {
    set_round(enums::round::preflop);
    set_flop_dealt(false);
  }

  engine_traits engine_traits_;
  enums::position current_{ enums::position::sb };
  enums::round round_{ enums::round::preflop };
  bool flop_dealt_{ false };
  std::vector< player > players_;
};
} // namespace pokerengine
#endif // POKERENGINE_ENGINE_HPP
