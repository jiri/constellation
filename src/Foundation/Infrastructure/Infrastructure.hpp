#pragma once

#include <Foundation/Infrastructure/Capabilities.hpp>
#include <Foundation/Infrastructure/Wiring.hpp>

struct Connection {
  Wiring::Port* a = nullptr;
  Wiring::Port* b = nullptr;
  Capabilities capabilities;
};
