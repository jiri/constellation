#include <Foundation/Debugger.hpp>

#include <Foundation/Universe.hpp>
#include <Foundation/Systems/Text.hpp>

Debugger::Debugger(Component* c, std::string portName)
  : component { c }
  , port { std::move(portName) }
{
  addCommand("help", [this] {
    std::string message;
    message += fmt::format("Debug commands available on '{}':\n", component->name());
    for (auto& pair : commands) {
      message += fmt::format("  {} {}\n", pair.first, pair.second.args);
    }
    component->universe->get<TextSystem>().send(&component->port(port), message);
  });
}

void Debugger::update() {
  while (auto s = component->universe->get<TextSystem>().receive(&component->port(port))) {
    std::istringstream iss(*s);

    std::vector<std::string> tokens {
        std::istream_iterator<std::string>{iss},
        std::istream_iterator<std::string>{}
    };

    if (!tokens.empty()) {
      try {
        if (commands.count(tokens[0]) != 0) {
          auto res = commands[tokens[0]].callback({ ++tokens.begin(), tokens.end() });

          if (res) {
            component->universe->get<TextSystem>().send(&component->port(port), *res);
          }
        }
        else {
          component->universe->get<TextSystem>().send(&component->port(port), fmt::format("Unknown command '{}'", tokens[0]));
        }
      }
      catch (std::runtime_error& e) {
        component->universe->get<TextSystem>().send(&component->port(port), e.what());
      }
    }
  }
}
