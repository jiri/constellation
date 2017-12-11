#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <type_traits>

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

class Debugger {
  using Command = std::function<std::string(const std::vector<std::string>&)>;
public:
  Debugger(Component* c, std::string portName);

  template <typename F>
  void addCommand(const std::string& name, F&& f) {
    auto lambda = [f](const std::vector<std::string>& ps) -> std::string {
      auto args = parseTuple<typename function_traits<F>::argument_tuple_type>(ps);

      if constexpr (!std::is_same<typename function_traits<F>::result_type, void>::value) {
        typename function_traits<F>::result_type r = std::apply(f, args);
        return fmt::format("{}", r);
      } else {
        std::apply(f, args);
        return {};
      }
    };
    commands[name] = lambda;
  }

  void process();

private:
  Component* component;
  std::string port;

  std::unordered_map<std::string, Command> commands;
};
