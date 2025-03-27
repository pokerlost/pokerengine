//
// Created by copper_boy on 11/2/23.
//

#ifndef POKERENGINE_PYEVALUATION_HPP
#define POKERENGINE_PYEVALUATION_HPP

#include "evaluator/evaluation_result.hpp"
#include "evaluator/result.hpp"

#include "python/python.hpp"

namespace python {
auto setup_pyevaluation_all(py::module_ &module_) -> void {
  auto evaluation = module_.def_submodule("evaluation");

  py::class_< pokerengine::result >(evaluation, "Result", py::module_local())
                  .def(py::init< pokerengine::enums::combination, uint8_t, uint8_t, uint16_t >(),
                       py::arg("type"),
                       py::arg("major"),
                       py::arg("minor"),
                       py::arg("kickers"))
                  .def(py::init< uint8_t, uint8_t, uint8_t, uint16_t >(),
                       py::arg("type"),
                       py::arg("major"),
                       py::arg("minor"),
                       py::arg("kickers"))
                  .def(py::self == py::self, py::arg("other")) // NOLINT
                  .def(py::self != py::self, py::arg("other")) // NOLINT
                  .def(py::self > py::self, py::arg("other"))  // NOLINT
                  .def(py::self < py::self, py::arg("other"))  // NOLINT
                  .def("__str__", [](pokerengine::result &self) -> std::string { return std::string{ self }; })
                  .def("__int__",
                       [](pokerengine::result &self) -> pokerengine::types::bit_set {
                         return self.as_bitset();
                       })
                  .def_property_readonly("type", &pokerengine::result::get_type)
                  .def_property_readonly("minor_rank", &pokerengine::result::get_minor_rank)
                  .def_property_readonly("major_rank", &pokerengine::result::get_major_rank)
                  .def_property_readonly("kickers", &pokerengine::result::get_kickers)
                  .def_property_readonly("result", &pokerengine::result::get_result);

  evaluation.def("get_evaluation_result",
                 &pokerengine::get_evaluation_result,
                 py::arg("cards"),
                 py::arg("players"));
  evaluation.def("get_evaluation_result_one",
                 &pokerengine::get_evaluation_result_one,
                 py::arg("cards"),
                 py::arg("for_"));
}

auto setup_pyevaluation_main(py::module_ &module_) -> void {
  setup_pyevaluation_all(module_);
}
} // namespace python

#endif // POKERENGINE_PYEVALUATION_HPP
