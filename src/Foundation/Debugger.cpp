#include <Foundation/Debugger.hpp>

#include <Foundation/Systems/Text.hpp>

void Debugger::process() {
  while (auto s = component->universe->get<TextSystem>().receive(&component->port(port))) {
    std::istringstream iss(*s);

    std::vector<std::string> tokens {
        std::istream_iterator<std::string>{iss},
        std::istream_iterator<std::string>{}
    };

    addCommand("help", [this] {
      std::string message;
      message += fmt::format("Debug commands available on '{}':\n", component->name());
      for (auto& pair : commands) {
        message += fmt::format("  {}\n", pair.first);
      }
      component->universe->get<TextSystem>().send(&component->port(port), message);
    });

    if (!tokens.empty()) {
      try {
        if (commands.count(tokens[0]) != 0) {
          auto res = commands[tokens[0]]({ ++tokens.begin(), tokens.end() });

          if (!res.empty()) {
            component->universe->get<TextSystem>().send(&component->port(port), res);
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
