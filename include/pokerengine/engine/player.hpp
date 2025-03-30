#ifndef POKERENGINE_PLAYER_HPP
#define POKERENGINE_PLAYER_HPP

#include <cstdint>
#include <string>
#include <vector>

#include <magic_enum/magic_enum.hpp>

#include "enums.hpp"
#include "pokerengine.hpp"

namespace pokerengine {
struct player {
  uint32_t stack;
  uint32_t bet;
  uint32_t round_bet;
  enums::state state;

  std::string id;

  auto operator<=>(const player &other) const noexcept -> std::strong_ordering = delete;
  auto operator==(const player &other) const noexcept -> bool = default;

  explicit operator std::string() const {
    using namespace std::literals;

    return "Player<stack="s + std::to_string(stack) + ", round_bet="s + std::to_string(round_bet) +
                    ", bet="s + std::to_string(bet) + ", stack="s + std::to_string(stack) + ", state="s +
                    std::string{ magic_enum::enum_name(state) } + ", id="s + id + ">"s;
  }
};
} // namespace pokerengine
#endif // POKERENGINE_PLAYER_HPP
