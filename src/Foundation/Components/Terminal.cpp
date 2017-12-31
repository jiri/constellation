#include <Foundation/Components/Terminal.hpp>

#include <imgui.h>

#include <Foundation/Universe.hpp>
#include <Foundation/Systems/Text.hpp>

void Terminal::update() {
  while (auto s = this->universe->system<TextSystem>().receive(port("debug"))) {
    messages.push_back(*s);
    newMessage = true;
  }

  Component::update();
}

void Terminal::render() {
  ImGui::Begin("Terminal");

  ImGui::BeginChild("##text", { 0, -ImGui::GetItemsLineHeightWithSpacing() });
  for (auto& msg : this->messages) {
    ImGui::Text("%s", msg.c_str());
  }
  if (this->newMessage) {
    ImGui::SetScrollHere();
    this->newMessage = false;
  }
  ImGui::EndChild();

  ImGui::PushItemWidth(-1);
  if (ImGui::InputText("##input", this->buf, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
    this->messages.push_back(fmt::format("> {}", this->buf));
    this->newMessage = true;
    this->universe->system<TextSystem>().send(port("debug"), this->buf);
    ImGui::SetKeyboardFocusHere();
    this->buf[0] = '\0';
  }
  ImGui::PopItemWidth();

  ImGui::End();
}
