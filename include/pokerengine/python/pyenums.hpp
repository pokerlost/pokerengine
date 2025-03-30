#ifndef POKERENGINE_PYENUMS_HPP
#define POKERENGINE_PYENUMS_HPP

#include "enums.hpp"

#include "python/python.hpp"

namespace python {
auto setup_action_enum(py::module_ &module_) -> void {
  auto action = module_.def_submodule("action");

  py::enum_< pokerengine::enums::action >(action, "Action", py::module_local())
                  .value("FOLD", pokerengine::enums::action::fold)
                  .value("CHECK", pokerengine::enums::action::check)
                  .value("CALL", pokerengine::enums::action::call)
                  .value("BET", pokerengine::enums::action::bet)
                  .value("RAISE", pokerengine::enums::action::raise);
}

auto setup_combination_enum(py::module_ &module_) -> void {
  auto combination = module_.def_submodule("combination");

  py::enum_< pokerengine::enums::combination >(combination, "Combination", py::module_local())
                  .value("NO_PAIR", pokerengine::enums::combination::no_pair)
                  .value("ONE_PAIR", pokerengine::enums::combination::one_pair)
                  .value("TWO_PAIR", pokerengine::enums::combination::two_pair)
                  .value("THREE_OF_A_KIND", pokerengine::enums::combination::three_of_a_kind)
                  .value("STRAIGHT", pokerengine::enums::combination::straight)
                  .value("FLUSH", pokerengine::enums::combination::flush)
                  .value("FULL_HOUSE", pokerengine::enums::combination::full_house)
                  .value("FOUR_OF_A_KIND", pokerengine::enums::combination::four_of_a_kind);
}

auto setup_position_enum(py::module_ &module_) -> void {
  auto position = module_.def_submodule("position");

  py::enum_< pokerengine::enums::position >(position, "Position", py::module_local())
                  .value("SB", pokerengine::enums::position::sb)
                  .value("BB", pokerengine::enums::position::bb)
                  .value("UTG", pokerengine::enums::position::utg)
                  .value("LWJ", pokerengine::enums::position::lwj)
                  .value("HIJ", pokerengine::enums::position::hij)
                  .value("COF", pokerengine::enums::position::cof)
                  .value("BTN", pokerengine::enums::position::btn);
}

auto setup_rank_enum(py::module_ &module_) -> void {
  auto rank = module_.def_submodule("rank");

  py::enum_< pokerengine::enums::rank >(rank, "Rank", py::module_local())
                  .value("TWO", pokerengine::enums::rank::two)
                  .value("THREE", pokerengine::enums::rank::three)
                  .value("FOUR", pokerengine::enums::rank::four)
                  .value("FIVE", pokerengine::enums::rank::five)
                  .value("SIX", pokerengine::enums::rank::six)
                  .value("SEVEN", pokerengine::enums::rank::seven)
                  .value("EIGHT", pokerengine::enums::rank::eight)
                  .value("NINE", pokerengine::enums::rank::nine)
                  .value("TEN", pokerengine::enums::rank::ten)
                  .value("JACK", pokerengine::enums::rank::jack)
                  .value("QUEEN", pokerengine::enums::rank::queen)
                  .value("KING", pokerengine::enums::rank::king)
                  .value("ACE", pokerengine::enums::rank::ace);
}

auto setup_round_enum(py::module_ &module_) -> void {
  auto round = module_.def_submodule("round");

  py::enum_< pokerengine::enums::round >(round, "Round", py::module_local())
                  .value("PREFLOP", pokerengine::enums::round::preflop)
                  .value("FLOP", pokerengine::enums::round::flop)
                  .value("TURN", pokerengine::enums::round::turn)
                  .value("RIVER", pokerengine::enums::round::river)
                  .value("SHOWDOWN", pokerengine::enums::round::showdown);
}

auto setup_state_enum(py::module_ &module_) -> void {
  auto state = module_.def_submodule("state");

  py::enum_< pokerengine::enums::state >(state, "State", py::module_local())
                  .value("INIT", pokerengine::enums::state::init)
                  .value("OUT", pokerengine::enums::state::out)
                  .value("ALIVE", pokerengine::enums::state::alive)
                  .value("ALLIN", pokerengine::enums::state::allin);
}

auto setup_suit_enum(py::module_ &module_) -> void {
  auto suit = module_.def_submodule("suit");

  py::enum_< pokerengine::enums::suit >(suit, "Suit", py::module_local())
                  .value("CLUBS", pokerengine::enums::suit ::clubs)
                  .value("DIAMONDS", pokerengine::enums::suit ::diamonds)
                  .value("HEARTS", pokerengine::enums::suit ::hearts)
                  .value("SPADES", pokerengine::enums::suit ::spades);
}

auto setup_pyenums_all(py::module_ &module_) -> void {
  auto enums = module_.def_submodule("enums");

  setup_action_enum(enums);
  setup_combination_enum(enums);
  setup_position_enum(enums);
  setup_rank_enum(enums);
  setup_round_enum(enums);
  setup_state_enum(enums);
  setup_suit_enum(enums);
}

auto setup_pyenums_main(py::module_ &module_) -> void {
  setup_pyenums_all(module_);
}
} // namespace python
#endif // POKERENGINE_PYENUMS_HPP
