//
// Created by copper_boy on 11/2/23.
//

#ifndef POKERENGINE_POKERENGINE_HPP
#define POKERENGINE_POKERENGINE_HPP

#include <stdexcept>
#include <string>

namespace pokerengine {
namespace v1 {
constexpr std::string version = "1.9";
}

namespace actual = v1;

class exception : std::exception {
  public:
  explicit exception(const std::string &message) : message_{ message } {
  }

  protected:
  std::string class_name_ = "exception";

  private:
  std::string message_;
};
} // namespace pokerengine
#endif // POKERENGINE_POKERENGINE_HPP
