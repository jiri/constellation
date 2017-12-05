#include <Foundation/Infrastructure/Infrastructure.hpp>

#include <Foundation/Universe.hpp>

std::string Port::name() const {
  return component->nameOf(this);
}
