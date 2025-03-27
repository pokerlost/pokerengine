//
// Created by copper_boy on 11/9/23.
//

#ifndef POKERENGINE_CONSTANTS_HPP
#define POKERENGINE_CONSTANTS_HPP

#include <array>
#include <cstdint>
#include <string>

#include "pokerengine.hpp"

namespace pokerengine::constants {
const uint8_t MASK_RANKS_NUMBER = 0b0000'1111;
const uint16_t MASK_RANKS = 0b0001'1111'1111'1111;

const uint8_t RANKS = 13;
const uint8_t SUITS = 4;

const uint8_t DECK_SIZE = RANKS * SUITS;

const uint8_t CARD_INDEX_MIN = 0;
const uint8_t CARD_INDEX_MAX = DECK_SIZE - 1;

const std::string CARDS_STRING =
                "2c3c4c5c6c7c8c9cTcJcQcKcAc2d3d4d5d6d7d8d9dTdJdQdKdAd2h3h4h5h6h7h8h9hThJhQh"
                "KhAh2s3s4s5s6s7s8s9sTsJsQsKsAs";

const uint64_t CARD_SET_FULL = 0xFFFFFFFFFFFFFFFF >> (64 - DECK_SIZE);

const uint8_t BOARD_SIZE = 5;
const uint8_t HAND_SIZE = 2;

const uint8_t MIN_PLAYERS = 2;
const uint8_t MAX_PLAYERS = 7;

template < size_t A = 0, size_t B = 1 >
  requires(A >= 0 && B > 0 && A < B)
const float RAKE = 1.0f * A / B;
template < size_t A = 0, size_t B = 1 >
  requires(A >= 0 && B > 0 && A < B)
const float RAKE_MULTI = 1.0f - RAKE< A, B >;

const uint8_t OFFSET_MINOR = RANKS;
const uint8_t OFFSET_MAJOR = OFFSET_MINOR + 4;
const uint8_t OFFSET_TYPE = OFFSET_MAJOR + 4;

const std::array< const std::string, 9 > COMBINATIONS = {
  "high card,", "a pair of",     "two pairs,",      "three of a kind,", "a straight,",
  "a flush,",   "a full house,", "four of a kind,", "a straight flush,"
};
} // namespace pokerengine::constants

#endif // POKERENGINE_CONSTANTS_HPP
