//
// Created by copper_boy on 11/9/23.
//

#ifndef POKERENGINE_PYCONSTANTS_HPP
#define POKERENGINE_PYCONSTANTS_HPP

#include "constants.hpp"

#include "python/python.hpp"

namespace python {
auto setup_constants_all(py::module_ &module_) -> void {
  auto constants = module_.def_submodule("constants");

  constants.attr("MASK_RANKS_NUMBER") = pokerengine::constants::MASK_RANKS_NUMBER;
  constants.attr("MASK_RANKS") = pokerengine::constants::MASK_RANKS;
  constants.attr("RANKS") = pokerengine::constants::RANKS;
  constants.attr("SUITS") = pokerengine::constants::SUITS;
  constants.attr("DECK_SIZE") = pokerengine::constants::DECK_SIZE;
  constants.attr("CARD_INDEX_MIN") = pokerengine::constants::CARD_INDEX_MIN;
  constants.attr("CARD_INDEX_MAX") = pokerengine::constants::CARD_INDEX_MAX;
  constants.attr("CARDS_STRING") = pokerengine::constants::CARDS_STRING;
  constants.attr("CARD_SET_FULL") = pokerengine::constants::CARD_SET_FULL;
  constants.attr("BOARD_SIZE") = pokerengine::constants::BOARD_SIZE;
  constants.attr("HAND_SIZE") = pokerengine::constants::HAND_SIZE;
  constants.attr("MIN_PLAYERS") = pokerengine::constants::MIN_PLAYERS;
  constants.attr("MAX_PLAYERS") = pokerengine::constants::MAX_PLAYERS;
  constants.attr("RAKE01") = pokerengine::constants::RAKE< 0, 1 >;
  constants.attr("RAKE_MULTI01") = pokerengine::constants::RAKE_MULTI< 0, 1 >;
  constants.attr("OFFSET_MINOR") = pokerengine::constants::OFFSET_MINOR;
  constants.attr("OFFSET_MAJOR") = pokerengine::constants::OFFSET_MAJOR;
  constants.attr("OFFSET_TYPE") = pokerengine::constants::OFFSET_TYPE;
  constants.attr("COMBINATIONS") = pokerengine::constants::COMBINATIONS;
}

auto setup_pyconstants_main(py::module_ &module_) -> void {
  setup_constants_all(module_);
}
} // namespace python

#endif // POKERENGINE_CONSTANTS_HPP
