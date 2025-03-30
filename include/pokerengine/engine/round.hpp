#ifndef POKERENGINE_ROUND_HPP
#define POKERENGINE_ROUND_HPP

#include <algorithm>
#include <vector>

#include "player.hpp"
#include "pokerengine.hpp"

namespace pokerengine {
auto set_blinds(std::vector< player > &players, uint16_t sb_bet, uint16_t bb_bet) -> void {
  std::for_each(players.begin(), players.end(), [&, index = 0](auto &player) mutable -> void {
    player.state = enums::state::init;
    player.bet = 0;
    player.round_bet = 0;

    if (index < 2) {
      player.state = index == 0 ? enums::state::alive : enums::state::init;
      player.bet = index == 0 ? sb_bet : bb_bet;

      if (player.bet > player.stack) {
        player.bet = player.stack;
      }

      player.stack -= player.bet;
      player.round_bet = player.bet;

      if (!player.stack) {
        player.state = enums::state::allin;
      }
    }

    index++;
  });
}

auto get_next_round(enums::round round) -> enums::round {
  if (round == enums::round::showdown) {
    return enums::round::showdown;
  }

  return static_cast< enums::round >(static_cast< uint8_t >(round) + 1);
}
} // namespace pokerengine
#endif // POKERENGINE_ROUND_HPP
