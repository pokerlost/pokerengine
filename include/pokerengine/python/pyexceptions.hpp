#ifndef POKERENGINE_PYEXCEPTIONS_HPP
#define POKERENGINE_PYEXCEPTIONS_HPP

#include "engine/action.hpp"
#include "engine/engine.hpp"
#include "pokerengine.hpp"

#include "python/python.hpp"

namespace python {
auto setup_pyexceptions_all(py::module_ &module_) -> void {
  auto exceptions = module_.def_submodule("exceptions");

  py::class_< pokerengine::exceptions::actions_error >(exceptions, "ActionsError", py::module_local());
  py::class_< pokerengine::exceptions::engine_error >(exceptions, "EngineError", py::module_local());
}

auto setup_pyexceptions_main(py::module_ &module_) -> void {
  setup_pyexceptions_all(module_);
}
} // namespace python
#endif // POKERENGINE_PYEXCEPTIONS_HPP
