//
// Created by copper_boy on 7/4/23.
//

#ifndef POKERENGINE_EVALUATION_HPP
#define POKERENGINE_EVALUATION_HPP

#include <bit>
#include <cstdint>

#include "bits.hpp"
#include "card/cards.hpp"
#include "enums.hpp"
#include "evaluator/lookup_tables.hpp"
#include "evaluator/result.hpp"
#include "pokerengine.hpp"

namespace pokerengine {
auto evaluate_unsafe(const card_set &cs) noexcept -> result {
  uint64_t mask = cs.get_cards();
  uint16_t mask_c = (mask >> (0 * constants::RANKS)) & constants::MASK_RANKS;
  uint16_t mask_d = (mask >> (1 * constants::RANKS)) & constants::MASK_RANKS;
  uint16_t mask_h = (mask >> (2 * constants::RANKS)) & constants::MASK_RANKS;
  uint16_t mask_s = (mask >> (3 * constants::RANKS)) & constants::MASK_RANKS;

  {
    auto flush_or_straight_flush = [](uint16_t mask_flush) -> result {
      auto x = poker_table_straight[mask_flush];
      if (x > 0) {
        return result{ enums::combination::straight_flush, x, 0, 0 };
      } else {
        return result{ enums::combination::flush, 0, 0, poker_table_top5[mask_flush] };
      }
    };

    if (std::popcount(mask_c) >= 5) {
      return flush_or_straight_flush(mask_c);
    } else if (std::popcount(mask_d) >= 5) {
      return flush_or_straight_flush(mask_d);
    } else if (std::popcount(mask_h) >= 5) {
      return flush_or_straight_flush(mask_h);
    } else if (std::popcount(mask_s) >= 5) {
      return flush_or_straight_flush(mask_s);
    }
  }

  uint16_t mask_all_cards = mask_c | mask_d | mask_h | mask_s;

  if (uint16_t mask_quads = (mask_c & mask_d & mask_h & mask_s); mask_quads) {
    return result{ enums::combination::four_of_a_kind,
                   bits::cross_idx_high16(mask_quads),
                   0,
                   static_cast< uint16_t >(
                                   (uint16_t(1) << bits::cross_idx_high16(mask_all_cards & ~mask_quads))) };
  }

  uint16_t mask_trips = ((mask_c & mask_d) | (mask_h & mask_s)) & ((mask_c & mask_h) | (mask_d & mask_s));

  if (mask_trips) {
    if (std::popcount(mask_trips) > 1) {
      return result{ enums::combination::full_house,
                     bits::cross_idx_high16(mask_trips),
                     bits::cross_idx_low16(mask_trips),
                     0 };
    }

    if (const uint16_t mask_pair_fh = (mask_all_cards ^ (mask_c ^ mask_d ^ mask_h ^ mask_s)); mask_pair_fh) {
      return result{ enums::combination::full_house,
                     bits::cross_idx_high16(mask_trips),
                     bits::cross_idx_high16(mask_pair_fh),
                     0 };
    }
  }

  auto rank_straight = poker_table_straight[mask_all_cards];
  if (rank_straight > 0) {
    return result{ enums::combination::straight, rank_straight, 0, 0 };
  }

  if (mask_trips) {
    uint16_t mask_kickers = mask_all_cards & ~(mask_trips);
    auto high_kicker = bits::cross_idx_high16(mask_kickers);
    auto low_kicker = bits::cross_idx_high16(mask_kickers & ~(uint16_t(1) << high_kicker));
    return result{ enums::combination::three_of_a_kind,
                   bits::cross_idx_high16(mask_trips),
                   0,
                   static_cast< uint16_t >(uint16_t(1) << high_kicker | uint16_t(1) << low_kicker) };
  }

  uint16_t mask_pair = (mask_all_cards ^ (mask_c ^ mask_d ^ mask_h ^ mask_s));
  if (const auto num_pairs = std::popcount(mask_pair); num_pairs > 1) {
    auto high_rank = bits::cross_idx_high16(mask_pair);
    auto low_rank = bits::cross_idx_high16(mask_pair & ~(uint16_t(1) << high_rank));
    auto kicker_rank = bits::cross_idx_high16(
                    mask_all_cards & ~(uint16_t(1) << high_rank | uint16_t(1) << low_rank));
    return result{
      enums::combination::two_pair, high_rank, low_rank, static_cast< uint16_t >(uint16_t(1) << kicker_rank)
    };
  } else if (num_pairs > 0) {
    uint16_t mask_kickers = mask_all_cards & ~(mask_pair);
    return result{
      enums::combination::one_pair, bits::cross_idx_high16(mask_pair), 0, poker_table_top3[mask_kickers]
    };
  }

  return result{ enums::combination::no_pair, 0, 0, poker_table_top5[mask_all_cards] };
}
} // namespace pokerengine

#endif // POKERENGINE_EVALUATION_HPP
