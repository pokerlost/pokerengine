//
// Created by copper_boy on 10/9/23.
//

#ifndef POKERENGINE_EVALUATION_RESULT_HPP
#define POKERENGINE_EVALUATION_RESULT_HPP

#include <cstdint>
#include <utility>
#include <vector>

#include "card/cards.hpp"
#include "evaluator/evaluation.hpp"
#include "evaluator/result.hpp"
#include "pokerengine.hpp"

namespace pokerengine {
auto get_evaluation_result(const cards &cards, const std::vector< uint8_t > &players)
                -> std::vector< std::pair< result, uint8_t > > {
  std::vector< std::pair< result, uint8_t > > winners;
  std::for_each(players.cbegin(), players.cend(), [&](auto const &player) {
    winners.emplace_back(
                    evaluate_unsafe(card_set{ cards.get_board() }.combine(
                                    cards.get_hands()[player].as_cardset())),
                    player);
  });
  std::sort(winners.begin(), winners.end(), [&](const auto &lhs, const auto &rhs) {
    return lhs.first > rhs.first;
  });
  auto first_non_winner = std::find_if(winners.cbegin() + 1, winners.cend(), [&](const auto &pair) {
    return pair.first < winners[0].first;
  });
  auto dist = std::distance(winners.cbegin(), first_non_winner);

  winners.resize(dist, std::make_pair(result(0, 0, 0, 0), 0));

  return winners;
}

auto get_evaluation_result_one(const cards &cards, uint8_t from) -> result {
  return evaluate_unsafe(card_set{ cards.get_board() }.combine(cards.get_hands()[from].as_cardset()));
}
} // namespace pokerengine

#endif // POKERENGINE_EVALUATION_RESULT_HPP
