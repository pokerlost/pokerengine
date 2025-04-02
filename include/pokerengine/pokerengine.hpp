#ifndef POKERENGINE_POKERENGINE_HPP
#define POKERENGINE_POKERENGINE_HPP

#include <stdexcept>
#include <string>
#include <utility>

namespace pokerengine {
namespace exceptions {
class exception : std::exception {
  public:
  explicit exception(const std::string &message) : message_{ message } {
  }

  explicit operator std::string() const {
    using namespace std::literals;

    return get_class_name() + "<message="s + get_message() + ">"s;
  }

  [[nodiscard]] auto get_class_name() const noexcept -> std::string {
    return class_name_;
  }

  [[nodiscard]] auto get_message() const noexcept -> std::string {
    return message_;
  }

  protected:
  std::string class_name_ = "exception";

  private:
  std::string message_;
};
} // namespace exceptions

constexpr std::string version = "1.9.1";
} // namespace pokerengine
#endif // POKERENGINE_POKERENGINE_HPP
