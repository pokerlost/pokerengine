#ifndef POKERENGINE_POT_HPP
#define POKERENGINE_POT_HPP

#include <algorithm>
#include <cstdint>
#include <vector>

#include "card/cards.hpp"
#include "evaluator/evaluation_result.hpp"
#include "player.hpp"
#include "pokerengine.hpp"

namespace pokerengine {
auto get_chips_to_return(const std::vector< player > &players, uint32_t highest_bet)
                -> std::pair< enums::position, uint32_t > {
  if (std::count_if(players.cbegin(), players.cend(), [&](const auto &element) -> bool {
        return element.bet == highest_bet;
      }) < 2) {
    std::vector< uint32_t > chips_bet;
    std::for_each(players.cbegin(), players.cend(), [&](const auto &element) -> void {
      chips_bet.push_back(element.bet);
    });


    auto position = std::distance(
                    players.cbegin(),
                    std::find_if(players.cbegin(), players.cend(), [&](const auto &element) -> bool {
                      return element.bet == highest_bet;
                    }));

    std::sort(chips_bet.begin(), chips_bet.end(), std::greater{});
    return std::make_pair(enums::position(position), highest_bet - chips_bet[1]);
  } else {
    return std::make_pair(enums::position{ 0 }, 0);
  }
}

auto get_chips_bet(const std::vector< player > &players, uint32_t highest_bet) -> std::vector< uint32_t > {
  std::vector< uint32_t > chips;
  for (const auto &player : players) {
    chips.push_back(player.bet);
  }

  auto chips_return = get_chips_to_return(players, highest_bet);
  if (chips_return.second == 0) {
    return chips;
  }
  chips[static_cast< uint8_t >(chips_return.first)] -= chips_return.second;
  return chips;
}

auto get_all_pots(const std::vector< player > &players, uint32_t highest_bet)
                -> std::vector< std::tuple< std::vector< uint8_t >, uint32_t, uint32_t > > {
  auto chips_bet = get_chips_bet(players, highest_bet);

  std::vector< std::pair< uint32_t, uint8_t > > chips_and_players;
  for (size_t i = 0; i < chips_bet.size(); i++) {
    chips_and_players.emplace_back(chips_bet[i], i);
  }
  std::sort(chips_and_players.begin(), chips_and_players.end(), [](const auto &lhs, const auto &rhs) -> bool {
    return lhs.first > rhs.first;
  });

  std::vector< uint8_t > main_pot_players;
  std::vector< std::tuple< std::vector< uint8_t >, uint32_t, uint32_t > > pots;

  uint32_t upper = chips_and_players[0].first;
  std::for_each(chips_and_players.cbegin(), chips_and_players.cend(), [&](const auto &pair) -> void {
    if (players[pair.second].state == enums::state::out) {
      return;
    } else if (uint32_t lower = chips_bet[pair.second]; lower == upper) {
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
auto get_adjust_pot(const std::vector< player > &players, uint32_t highest_bet, bool flop_dealt) -> uint32_t {
  std::vector< uint32_t > chips_bet;
  std::for_each(players.cbegin(), players.cend(), [&](auto const &element) {
    chips_bet.push_back(element.bet);
  });

  auto pot = std::reduce(chips_bet.cbegin(), chips_bet.cend());
  if (flop_dealt && constants::RAKE< A, B > != 0.0) {
    auto chips_returned = get_chips_to_return(players, highest_bet).second;
    return static_cast< uint32_t >((pot - chips_returned) * constants::RAKE< A, B > + chips_returned);
  } else {
    return pot;
  }
}

auto adjust_side_pot(const std::vector< player > &players, uint32_t upper_bound, uint32_t lower_bound) noexcept
                -> std::vector< uint32_t > {
  std::vector< uint32_t > result;
  for (auto const &player : players) {
    auto chips = player.bet;
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
                uint32_t upper_bound,
                uint32_t lower_bound) -> std::vector< int32_t > {
  auto winners = get_evaluation_result(cards, players);
  auto chips_adjusted = adjust_side_pot(ps, upper_bound, lower_bound);

  auto total_pot = static_cast< uint32_t >(
                  std::accumulate(chips_adjusted.cbegin(), chips_adjusted.cend(), 0u) *
                  (flop_dealt ? constants::RAKE_MULTI< A, B > : 1.0f));
  uint32_t amount_each_winner = total_pot / static_cast< uint32_t >(winners.size());

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
} // namespace pokerengine
#endif // POKERENGINE_POT_HPP
