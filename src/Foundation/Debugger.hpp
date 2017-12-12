#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <type_traits>
#include <optional>

#include <Foundation/Universe.hpp>
#include <Foundation/Infrastructures/Infrastructure.hpp>
#include <Util/Parse.hpp>

template <typename T>
struct function_traits
  : public function_traits<decltype(&T::operator())>
{};

template <typename C, typename R, typename... Ps>
struct function_traits<R(C::*)(Ps...) const> {
  using result_type = R;
  using argument_tuple_type = std::tuple<Ps...>;
};

struct Command {
  std::string args;
  std::function<std::optional<std::string>(const std::vector<std::string>&)> callback;
};

class Debugger {
public:
  Debugger(Component* c, std::string portName);

  template <typename F>
  void addCommand(const std::string& name, F&& f) {
    auto lambda = [f](const std::vector<std::string>& ps) -> std::optional<std::string> {
      auto args = parseTuple<typename function_traits<F>::argument_tuple_type>(ps);

      if constexpr (!std::is_same<typename function_traits<F>::result_type, void>::value) {
        typename function_traits<F>::result_type r = std::apply(f, args);
        return fmt::format("{}", r);
      } else {
        std::apply(f, args);
        return std::nullopt;
      }
    };
    commands[name] = {
        tupleToString<typename function_traits<F>::argument_tuple_type>(),
        lambda,
    };
  }

  void update();

private:
  Component* component;
  std::string port;

  std::unordered_map<std::string, Command> commands;
};
