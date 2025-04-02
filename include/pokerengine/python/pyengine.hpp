#ifndef POKERENGINE_PYENGINE_HPP
#define POKERENGINE_PYENGINE_HPP

#include <pybind11/operators.h>

#include "engine/engine.hpp"

#include "python/python.hpp"

namespace python {
template < uint8_t A = 0, uint8_t B = 1 >
  requires(A >= 0 && B > 0 && A < B)
auto setup_pyengine_template(py::module_ &module_, const std::string &pyclass_postfix) -> void {
  py::class_< pokerengine::engine< A, B > >(module_, ("Engine" + pyclass_postfix).c_str(), py::module_local())
                  .def(py::init< const pokerengine::engine_traits & >(), py::arg("engine_traits"))
                  .def(py::init< const pokerengine::engine_traits &,
                                 pokerengine::enums::position,
                                 pokerengine::enums::round,
                                 bool,
                                 const std::vector< pokerengine::player > & >(),
                       py::arg("engine_traits"),
                       py::arg("current"),
                       py::arg("round"),
                       py::arg("flop_dealt"),
                       py::arg("players"))
                  .def("start", &pokerengine::engine< A, B >::start, py::arg("is_new_game"))
                  .def("stop", &pokerengine::engine< A, B >::stop)
                  .def("add_player", &pokerengine::engine< A, B >::join_player, py::arg("stack"), py::arg("id"))
                  .def("pay", &pokerengine::engine< A, B >::pay, py::arg("cards"))
                  .def("pay_noshowdown", &pokerengine::engine< A, B >::pay_noshowdown)
                  .def("execute", &pokerengine::engine< A, B >::execute, py::arg("player_action"))
                  .def_property("engine_traits",
                                &pokerengine::engine< A, B >::get_engine_traits,
                                &pokerengine::engine< A, B >::set_engine_traits)
                  .def_property_readonly("current", &pokerengine::engine< A, B >::get_current)
                  .def_property_readonly("current_player", &pokerengine::engine< A, B >::get_current_player)
                  .def_property_readonly("flop_dealt", &pokerengine::engine< A, B >::get_flop_dealt)
                  .def_property_readonly("highest_bet", &pokerengine::engine< A, B >::get_highest_bet)
                  .def_property_readonly("highest_game_bet", &pokerengine::engine< A, B >::get_highest_game_bet)
                  .def_property_readonly("players", &pokerengine::engine< A, B >::get_players)
                  .def_property_readonly("possible_actions", &pokerengine::engine< A, B >::get_possible_actions)
                  .def_property_readonly("round", &pokerengine::engine< A, B >::get_round)
                  .def_property_readonly("pot", &pokerengine::engine< A, B >::get_pot)
                  .def_property_readonly("showdown", &pokerengine::engine< A, B >::is_showdown)
                  .def_property_readonly("terminal_state", &pokerengine::engine< A, B >::in_terminal_state);
}

auto setup_pyengine_notemplate(py::module_ &module_) -> void {
  py::class_< pokerengine::engine_traits >(module_, "EngineTraits", py::module_local())
                  .def(py::init< uint16_t, uint16_t, uint8_t, uint32_t >(),
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
                  .def(py::init< uint32_t, uint32_t, uint32_t, pokerengine::enums::state, std::string >(),
                       py::arg("stack"),
                       py::arg("bet"),
                       py::arg("round_bet"),
                       py::arg("state"),
                       py::arg("id"))
                  .def(py::self == py::self, py::arg("other")) // NOLINT
                  .def("__str__", [](pokerengine::player &self) -> std::string { return std::string{ self }; })
                  .def_readwrite("stack", &pokerengine::player::stack)
                  .def_readwrite("bet", &pokerengine::player::bet)
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

template < uint8_t A = 0, uint8_t B = 1 >
  requires(A >= 0 && B > 0 && A < B)
auto setup_pyengine_all(py::module_ module_, const std::string &pyclass_postfix) -> void {
  auto engine = module_.def_submodule("engine");

  setup_pyengine_notemplate(engine);
  setup_pyengine_template< A, B >(engine, pyclass_postfix);
}

auto setup_pyengine_main(py::module_ &module_) -> void {
  setup_pyengine_all< 0, 1 >(module_, std::string{ "Rake01" });
}
} // namespace python
#endif // POKERENGINE_PYENGINE_HPP
