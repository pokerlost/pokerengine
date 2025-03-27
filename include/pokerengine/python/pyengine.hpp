//
// Created by copper_boy on 11/15/23.
//

#ifndef POKERENGINE_PYENGINE_HPP
#define POKERENGINE_PYENGINE_HPP

#include <pybind11/operators.h>

#include "engine.hpp"

#include "python/python.hpp"

namespace python {
template < uint8_t A, uint8_t B >
auto setup_pyengine_template(py::module_ &module_, const std::string &pyclass_postfix) -> void {
  py::class_< pokerengine::actual::engine_detail< pokerengine::engine< A, B > > >( // NOLINT
                  module_,
                  ("_EngineDetail" + pyclass_postfix).c_str(),
                  py::module_local());
  py::class_< pokerengine::engine< A, B > >(module_, ("Engine" + pyclass_postfix).c_str(), py::module_local())
                  .def(py::init< const pokerengine::engine_traits & >(), py::arg("traits"))
                  .def("load",
                       &pokerengine::engine< A, B >::load,
                       py::arg("traits"),
                       py::arg("players"),
                       py::arg("position"),
                       py::arg("round"),
                       py::arg("flop_dealt"))
                  .def("start", &pokerengine::engine< A, B >::start, py::arg("new_game"))
                  .def("stop", &pokerengine::engine< A, B >::stop)
                  .def_property("traits",
                                &pokerengine::engine< A, B >::get_engine_traits,
                                &pokerengine::engine< A, B >::set_engine_traits)
                  .def_readonly("actions", &pokerengine::engine< A, B >::actions)
                  .def_readonly("players", &pokerengine::engine< A, B >::players)
                  .def_readonly("positions", &pokerengine::engine< A, B >::positions)
                  .def_readonly("pot", &pokerengine::engine< A, B >::pot)
                  .def_readonly("round", &pokerengine::engine< A, B >::round);
  py::class_< pokerengine::actions_manager< pokerengine::engine< A, B > > >(
                  module_, ("Actions" + pyclass_postfix).c_str(), py::module_local())
                  .def(py::init< pokerengine::engine< A, B > & >(), py::arg("engine"))
                  .def("execute",
                       &pokerengine::actions_manager< pokerengine::engine< A, B > >::execute,
                       py::arg("action"))
                  .def_property_readonly(
                                  "actions",
                                  &pokerengine::actions_manager< pokerengine::engine< A, B > >::get_possible_actions);
  py::class_< pokerengine::players_manager< pokerengine::engine< A, B > > >(
                  module_, ("Players" + pyclass_postfix).c_str(), py::module_local())
                  .def(py::init< pokerengine::engine< A, B > & >(), py::arg("engine"))
                  .def("get_player",
                       &pokerengine::players_manager< pokerengine::engine< A, B > >::get_player,
                       py::arg("index"))
                  .def("set_players",
                       &pokerengine::players_manager< pokerengine::engine< A, B > >::set_players,
                       py::arg("players"))
                  .def("add_player",
                       &pokerengine::players_manager< pokerengine::engine< A, B > >::add_player,
                       py::arg("stack"),
                       py::arg("id"))
                  .def("remove_player",
                       &pokerengine::players_manager< pokerengine::engine< A, B > >::remove_player,
                       py::arg("id"))
                  .def_property_readonly(
                                  "players",
                                  &pokerengine::players_manager< pokerengine::engine< A, B > >::get_players);
  py::class_< pokerengine::positions_manager< pokerengine::engine< A, B > > >(
                  module_, ("Positions" + pyclass_postfix).c_str(), py::module_local())
                  .def(py::init< pokerengine::engine< A, B > & >(), py::arg("engine"))
                  .def("set_next_current",
                       &pokerengine::positions_manager< pokerengine::engine< A, B > >::set_next_current)
                  .def("set_next_round_player",
                       &pokerengine::positions_manager< pokerengine::engine< A, B > >::set_next_round_player)
                  .def("actionable",
                       &pokerengine::positions_manager< pokerengine::engine< A, B > >::get_actionable)
                  .def_property_readonly(
                                  "showdown",
                                  &pokerengine::positions_manager< pokerengine::engine< A, B > >::is_showdown)
                  .def_property_readonly(
                                  "number_alive",
                                  &pokerengine::positions_manager< pokerengine::engine< A, B > >::get_number_alive)
                  .def_property_readonly(
                                  "future_actionable",
                                  &pokerengine::positions_manager< pokerengine::engine< A, B > >::get_future_actionable)
                  .def_property_readonly(
                                  "player",
                                  py::overload_cast<>(&pokerengine::positions_manager<
                                                      pokerengine::engine< A, B > >::get_player))
                  .def_property_readonly(
                                  "player",
                                  py::overload_cast<>(
                                                  &pokerengine::positions_manager< pokerengine::engine< A, B > >::get_player,
                                                  py::const_))
                  .def_property("current",
                                &pokerengine::positions_manager< pokerengine::engine< A, B > >::get_current,
                                &pokerengine::positions_manager< pokerengine::engine< A, B > >::set_current);
  py::class_< pokerengine::pot_manager< pokerengine::engine< A, B >, A, B > >(
                  module_, ("Pot" + pyclass_postfix).c_str(), py::module_local())
                  .def(py::init< pokerengine::engine< A, B > & >(), py::arg("engine"))
                  .def("pay",
                       &pokerengine::pot_manager< pokerengine::engine< A, B >, A, B >::pay,
                       py::arg("cards"))
                  .def("pay_noshowdown",
                       &pokerengine::pot_manager< pokerengine::engine< A, B >, A, B >::pay_noshowdown)
                  .def("pot", &pokerengine::pot_manager< pokerengine::engine< A, B >, A, B >::pot)
                  .def("pot_rake", &pokerengine::pot_manager< pokerengine::engine< A, B >, A, B >::pot_rake)
                  .def_property_readonly(
                                  "game_highest_bet",
                                  &pokerengine::pot_manager< pokerengine::engine< A, B >, A, B >::get_highest_game_bet)
                  .def_property_readonly(
                                  "round_highest_bet",
                                  &pokerengine::pot_manager< pokerengine::engine< A, B >, A, B >::get_highest_bet);
  py::class_< pokerengine::round_manager< pokerengine::engine< A, B > > >(
                  module_, ("Round" + pyclass_postfix).c_str(), py::module_local())
                  .def(py::init< pokerengine::engine< A, B > & >(), py::arg("engine"))
                  .def("reset", &pokerengine::round_manager< pokerengine::engine< A, B > >::reset)
                  .def_property_readonly(
                                  "terminal_state",
                                  &pokerengine::round_manager< pokerengine::engine< A, B > >::in_terminal_state)
                  .def_property("round",
                                &pokerengine::round_manager< pokerengine::engine< A, B > >::get_round,
                                &pokerengine::round_manager< pokerengine::engine< A, B > >::set_round)
                  .def_property("flop_dealt",
                                &pokerengine::round_manager< pokerengine::engine< A, B > >::get_flop_dealt,
                                &pokerengine::round_manager< pokerengine::engine< A, B > >::set_flop_dealt);
}

auto setup_pyengine_notemplate(py::module_ &module_) -> void {
  py::class_< pokerengine::engine_traits >(module_, "EngineTraits", py::module_local())
                  .def(py::init< int32_t, int32_t, uint8_t, int32_t >(),
                       py::arg("sb_bet"),
                       py::arg("bb_bet"),
                       py::arg("bb_mult"),
                       py::arg("min_raise"))
                  .def_property("sb_bet",
                                &pokerengine::engine_traits::get_sb_bet,
                                &pokerengine::engine_traits::set_sb_bet)
                  .def_property("bb_bet",
                                &pokerengine::engine_traits::get_bb_bet,
                                &pokerengine::engine_traits::set_bb_bet)
                  .def_property("bb_mult",
                                &pokerengine::engine_traits::get_bb_mult,
                                &pokerengine::engine_traits::set_bb_mult)
                  .def_property("min_raise",
                                &pokerengine::engine_traits::get_min_raise,
                                &pokerengine::engine_traits::set_min_raise);
  py::class_< pokerengine::player >(module_, "Player", py::module_local())
                  .def(py::init< bool, int32_t, int32_t, int32_t, int32_t, pokerengine::enums::state, std::string >(),
                       py::arg("is_left"),
                       py::arg("stack"),
                       py::arg("behind"),
                       py::arg("front"),
                       py::arg("round_bet"),
                       py::arg("state"),
                       py::arg("id"))
                  .def(py::self == py::self, py::arg("other")) // NOLINT
                  .def("__str__", [](pokerengine::player &self) -> std::string { return std::string{ self }; })
                  .def_readwrite("is_left", &pokerengine::player::is_left)
                  .def_readwrite("stack", &pokerengine::player::stack)
                  .def_readwrite("behind", &pokerengine::player::behind)
                  .def_readwrite("front", &pokerengine::player::front)
                  .def_readwrite("round_bet", &pokerengine::player::round_bet)
                  .def_readwrite("state", &pokerengine::player::state)
                  .def_readwrite("id", &pokerengine::player::id);
  py::class_< pokerengine::player_action >(module_, "PlayerAction", py::module_local())
                  .def(py::init< int32_t, pokerengine::enums::action, pokerengine::enums::position >(),
                       py::arg("amount"),
                       py::arg("action"),
                       py::arg("position"))
                  .def(py::self == py::self, py::arg("other")) // NOLINT
                  .def("__str__",
                       [](pokerengine::player_action &self) -> std::string { return std::string{ self }; })
                  .def_readwrite("amount", &pokerengine::player_action::amount)
                  .def_readwrite("action", &pokerengine::player_action::action)
                  .def_readwrite("position", &pokerengine::player_action::position);
}

template < uint8_t A, uint8_t B >
auto setup_pyengine_all(py::module_ module_, const std::string &pyclass_postfix) -> void {
  auto engine = module_.def_submodule("engine");

  setup_pyengine_notemplate(engine);
  setup_pyengine_template< A, B >(engine, pyclass_postfix);
}

auto setup_pyengine_main(py::module_ &module_) -> void {
  setup_pyengine_all< 0, 1 >(module_, "Rake01");
}
} // namespace python

#endif // POKERENGINE_ENGINE_HPP
