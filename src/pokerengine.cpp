#include "card/card.hpp"

#include "python/pycard.hpp"
#include "python/pyconstants.hpp"
#include "python/pyengine.hpp"
#include "python/pyenums.hpp"
#include "python/pyevaluation.hpp"
#include "python/pyexceptions.hpp"
#include "python/pypokerengine.hpp"
#include "python/python.hpp"

namespace python {
auto setup_py_all(py::module_ &module_) -> void {
  setup_pyconstants_main(module_);
  setup_pyenums_main(module_);
  setup_pycard_main(module_);
  setup_pyevaluation_main(module_);
  setup_pyexceptions_main(module_);
  setup_pypokerengine_main(module_);
  setup_pyengine_main(module_);
}

auto setup_py_main(py::module_ &module_) -> void {
  setup_py_all(module_);
}
} // namespace python

PYBIND11_MODULE(pokerengine_core, module_) {
  module_.doc() = "Poker Library";
  python::setup_py_main(module_);

  module_.attr("__version__") = pokerengine::version;
}
