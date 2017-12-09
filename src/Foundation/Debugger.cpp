#include <Foundation/Debugger.hpp>

#include <Foundation/Systems/Text.hpp>

void Debugger::process() {
  while (auto s = component->universe->get<TextSystem>().receive(&component->port(port))) {
    std::istringstream iss(*s);

    std::vector<std::string> tokens {
        std::istream_iterator<std::string>{iss},
        std::istream_iterator<std::string>{}
    };

    try {
      if (!tokens.empty() && tokens[0] == "help") {
        std::string message;
        for (auto& pair : commands) {
          message += pair.first + "\n";
        }
        component->universe->get<TextSystem>().send(&component->port(port), message);
      }
      else if (!tokens.empty() && commands.count(tokens[0]) != 0) {
        auto res = commands[tokens[0]]({ ++tokens.begin(), tokens.end() });

        if (!res.empty()) {
          component->universe->get<TextSystem>().send(&component->port(port), res);
        }
      }
      else {
        component->universe->get<TextSystem>().send(&component->port(port), "Unknown command");
      }
    }
    catch (std::runtime_error& e) {
      component->universe->get<TextSystem>().send(&component->port(port), e.what());
    }
  }
}
