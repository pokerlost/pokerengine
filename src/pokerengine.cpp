//
// Created by copper_boy on 10/29/23.
//

#include "card/card.hpp"

#include "python/pycard.hpp"
#include "python/pyconstants.hpp"
#include "python/pyengine.hpp"
#include "python/pyenums.hpp"
#include "python/pyevaluation.hpp"
#include "python/python.hpp"

namespace python {
auto setup_all(py::module_ &module_) -> void {
  setup_pyconstants_main(module_);
  setup_pyenums_main(module_);
  setup_pycard_main(module_);
  setup_pyevaluation_main(module_);
  setup_pyengine_main(module_);
}

auto setup_main(py::module_ &module_) -> void {
  setup_all(module_);
}
} // namespace python

PYBIND11_MODULE(pokerengine_core, module_) {
  module_.doc() = "Poker Library";
  python::setup_main(module_);

  module_.attr("__version__") = pokerengine::actual::version;
}
