#pragma once

#include <Foundation/Systems/Picture.hpp>
#include <Foundation/Systems/Energy.hpp>
#include <Foundation/Systems/Text.hpp>

struct Capabilities {
  Picture::Capability picture;
  Energy::Capability energy;
  Text::Capability text;

  static Capabilities combine(const Capabilities& a, const Capabilities& b);
};
