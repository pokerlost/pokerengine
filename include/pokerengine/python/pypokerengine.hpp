#ifndef POKERENGINE_PYPOKERENGINE_HPP
#define POKERENGINE_PYPOKERENGINE_HPP

#include "pokerengine.hpp"

#include "python/python.hpp"

namespace python {
auto setup_pypokerengine_all(py::module_ &module_) -> void {
  auto pokerengine = module_.def_submodule("pokerengine");

  py::class_< pokerengine::exceptions::exception >(pokerengine, "PokerEngineError", py::module_local())
                  .def(py::init< std::string >(), py::arg("message"))
                  .def("__str__",
                       [](pokerengine::exceptions::exception &self) -> std::string {
                         return std::string{ self };
                       })
                  .def_property_readonly("class_name", &pokerengine::exceptions::exception::get_class_name)
                  .def_property_readonly("message", &pokerengine::exceptions::exception::get_message);
}

auto setup_pypokerengine_main(py::module_ &module_) -> void {
  setup_pypokerengine_all(module_);
}
} // namespace python
#endif // POKERENGINE_PYPOKERENGINE_HPP
