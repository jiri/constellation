#pragma once

#include <string>
#include <unordered_map>

#include <Foundation/Universe.hpp>
#include <Foundation/Infrastructures/Infrastructure.hpp>
#include <Util/Parse.hpp>

class Debugger {
  using Command = std::function<std::string(const std::vector<std::string>&)>;
  using VoidCommand = std::function<void(const std::vector<std::string>&)>;
  using CommandVoid = std::function<std::string()>;
public:
  Debugger(Component* c, std::string portName)
    : component { c }
    , port { std::move(portName) }
  { }

  void addCommand(const std::string& name, Command&& c);
  void addCommand(const std::string& name, VoidCommand&& c);
  void addCommand(const std::string& name, CommandVoid&& c);
  void process();

private:
  Component* component;
  std::string port;

  std::unordered_map<std::string, Command> commands;
};
