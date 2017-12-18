#include <Foundation/Systems/System.hpp>

#include <Foundation/Universe.hpp>

#include <imgui.h>

void System::update() {
  for (auto& connection : this->universe->connections) {
    if (this->filter(connection)) {
      this->swap(connection);
    }
  }
}

void System::timePassed(std::chrono::system_clock::duration d) {
  deltaCounter += d;
  const auto oneTick = std::chrono::microseconds(1000000 / ups);

  while (deltaCounter >= oneTick) {
    deltaCounter -= oneTick;
    if (this->active) {
      upsCounter++;
      update();
    }
  }

  upsTimeCounter += d;

  if (upsTimeCounter > std::chrono::seconds(1)) {
    actualUps = upsCounter / std::chrono::duration_cast<std::chrono::seconds>(upsTimeCounter).count();

    upsCounter = 0;
    upsTimeCounter = std::chrono::system_clock::duration::zero();
  }
}

void System::UI() {
  ImGui::Checkbox("Active", &this->active);
  ImGui::Text("UPS: %u / %f", ups, actualUps);
}
