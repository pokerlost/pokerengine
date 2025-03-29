//
// Created by user on 3/29/25.
//

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
auto get_chips_to_return(const std::vector< player > &players, int32_t highest_bet)
                -> std::pair< enums::position, int32_t > {
  if (std::count_if(players.cbegin(), players.cend(), [&](const auto &element) -> bool {
        return element.bet == highest_bet;
      }) < 2) {
    std::vector< uint32_t > chips_front;
    std::for_each(players.cbegin(), players.cend(), [&](const auto &element) -> void {
      chips_front.push_back(element.bet);
    });


    auto position = std::distance(
                    players.cbegin(),
                    std::find_if(players.cbegin(), players.cend(), [&](const auto &element) -> bool {
                      return element.bet == highest_bet;
                    }));

    std::sort(chips_front.begin(), chips_front.end(), std::greater{});
    return std::make_pair(enums::position(position), highest_bet - chips_front[1]);
  } else {
    return std::make_pair(enums::position{ 0 }, 0);
  }
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
} // namespace pokerengine
#endif // POKERENGINE_POT_HPP
