#pragma once

#include <list>

#include <Foundation/Components/Component.hpp>

class Terminal : public Component {
public:
  using Component::Component;

  void update() override;
  void render() override;

  std::string name() const override {
    return "terminal";
  }

private:
  bool newMessage = false;
  char buf[256] {};
  std::list<std::string> messages;
};
