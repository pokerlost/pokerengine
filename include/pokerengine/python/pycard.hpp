//
// Created by copper_boy on 11/1/23.
//

#ifndef POKERENGINE_PYCARD_HPP
#define POKERENGINE_PYCARD_HPP

#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include <span>

#include "card/card.hpp"
#include "card/cards.hpp"
#include "card/hand.hpp"

#include "python/python.hpp"

namespace python {
auto setup_pycard_all(py::module_ &module_) -> void {
  auto card = module_.def_submodule("card");

  py::class_< pokerengine::rank >(card, "Rank", py::module_local())
                  .def(py::init< pokerengine::enums::rank >(), py::arg("value"))
                  .def(py::init< char >(), py::arg("value"))
                  .def(py::self == py::self, py::arg("other")) // NOLINT
                  .def(py::self != py::self, py::arg("other")) // NOLINT
                  .def(py::self > py::self, py::arg("other"))  // NOLINT
                  .def(py::self < py::self, py::arg("other"))  // NOLINT
                  .def("__str__", [](pokerengine::rank &self) -> std::string { return std::string{ self }; })
                  .def("__int__", [](pokerengine::rank &self) -> int { return self.as_bitset(); })
                  .def_property_readonly("rank", &pokerengine::rank::get_value)
                  .def_property_readonly("enum", &pokerengine::rank::get_enum);
  py::class_< pokerengine::suit >(card, "Suit", py::module_local()); // NOLINT

  py::class_< pokerengine::card >(card, "Card", py::module_local())
                  .def(py::init< const pokerengine::rank &, const pokerengine::suit & >(),
                       py::arg("rank"),
                       py::arg("suit"))
                  .def(py::init< pokerengine::enums::rank, pokerengine::enums::suit >(),
                       py::arg("rank"),
                       py::arg("suit"))
                  .def(py::init< uint8_t >(), py::arg("card"))
                  .def(py::init< std::string_view >(), py::arg("value"))
                  .def(py::self == py::self, py::arg("other")) // NOLINT
                  .def(py::self != py::self, py::arg("other")) // NOLINT
                  .def(py::self > py::self, py::arg("other"))  // NOLINT
                  .def(py::self < py::self, py::arg("other"))  // NOLINT
                  .def("__str__", [](pokerengine::card &self) -> std::string { return std::string{ self }; })
                  .def("__int__",
                       [](pokerengine::card &self) -> pokerengine::types::bit_set { return self.as_bitset(); })
                  .def_property_readonly("card", &pokerengine::card::get_card)
                  .def_property_readonly("rank", &pokerengine::card::get_rank)
                  .def_property_readonly("suit", &pokerengine::card::get_suit);

  py::class_< pokerengine::card_set >(card, "CardSet", py::module_local())
                  .def(py::init())
                  .def(py::init< const std::span< const pokerengine::card > & >(), py::arg("cards"))
                  .def(py::init< uint64_t >(), py::arg("bitset"))
                  .def(py::init< std::string_view >(), py::arg("value"))
                  .def("__len__", &pokerengine::card_set::size)
                  .def(
                                  "__contains__",
                                  [](pokerengine::card_set &self, const pokerengine::card &v) -> bool {
                                    return self.contains(v);
                                  },
                                  py::arg("other"))
                  .def(
                                  "__contains__",
                                  [](pokerengine::card_set &self, const pokerengine::card_set &v) -> bool {
                                    return self.contains(v);
                                  },
                                  py::arg("other"))
                  .def(
                                  "__add__",
                                  [](pokerengine::card_set &self, const pokerengine::card &v)
                                                  -> pokerengine::card_set { return self.combine(v); },
                                  py::arg("other"))
                  .def(
                                  "__add__",
                                  [](pokerengine::card_set &self, const pokerengine::card_set &v)
                                                  -> pokerengine::card_set { return self.combine(v); },
                                  py::arg("other"))
                  .def("clear", &pokerengine::card_set::clear)
                  .def("fill", &pokerengine::card_set::fill)
                  .def("insert", &pokerengine::card_set::insert, py::arg("value"))
                  .def("join", &pokerengine::card_set::join, py::arg("value"))
                  .def(
                                  "remove",
                                  [](pokerengine::card_set &self, const pokerengine::card_set &v) -> void {
                                    self.remove(v);
                                  },
                                  py::arg("value"))
                  .def(
                                  "remove",
                                  [](pokerengine::card_set &self, const pokerengine::card_set &v) -> void {
                                    self.remove(v);
                                  },
                                  py::arg("value"))
                  .def_property("cards", &pokerengine::card_set::get_cards, &pokerengine::card_set::set_cards);

  py::class_< pokerengine::card_generator >(card, "CardGenerator", py::module_local())
                  .def(py::init< uint16_t >(), py::arg("seed") = 1927)
                  .def("generate", &pokerengine::card_generator::generate)
                  .def("generate_v", &pokerengine::card_generator::generate_v, py::arg("n"));

  py::class_< pokerengine::cards >(card, "Cards", py::module_local())
                  .def(py::init< const std::vector< std::string > &, const std::vector< std::string > & >(),
                       py::arg("board"),
                       py::arg("hands"))
                  .def(py::init< const std::vector< pokerengine::card > & >(), py::arg("cards"))
                  .def(py::self == py::self, py::arg("other")) // NOLINT
                  .def(py::self != py::self, py::arg("other")) // NOLINT
                  .def(py::self > py::self, py::arg("other"))  // NOLINT
                  .def(py::self < py::self, py::arg("other"))  // NOLINT
                  .def("__str__", [](pokerengine::cards &self) -> std::string { return std::string{ self }; })
                  .def("board_n", &pokerengine::cards::get_board_n)
                  .def_property_readonly("board", &pokerengine::cards::get_board)
                  .def_property_readonly("hands", &pokerengine::cards::get_hands);

  py::class_< pokerengine::hand >(card, "Hand", py::module_local())
                  .def(py::init< pokerengine::card, pokerengine::card >())
                  .def(py::init< uint8_t, uint8_t >())
                  .def(py::init< std::string_view >())
                  .def(py::self == py::self, py::arg("other")) // NOLINT
                  .def(py::self != py::self, py::arg("other")) // NOLINT
                  .def(py::self > py::self, py::arg("other"))  // NOLINT
                  .def(py::self < py::self, py::arg("other"))  // NOLINT
                  .def("__str__", [](pokerengine::hand &self) -> std::string { return std::string{ self }; })
                  .def("__int__",
                       [](pokerengine::hand &self) -> pokerengine::types::bit_set { return self.as_bitset(); })
                  .def("as_cardset", &pokerengine::hand::as_cardset)
                  .def_property_readonly("value", &pokerengine::hand::get_value);
}

auto setup_pycard_main(py::module_ &module_) -> void {
  setup_pycard_all(module_);
}
} // namespace python

#endif // POKERENGINE_PYCARD_HPP
