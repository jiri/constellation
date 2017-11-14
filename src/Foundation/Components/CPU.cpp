#include <Foundation/Components/CPU.hpp>

#include <imgui.h>
#include <imgui_internal.h>

int CPU::pop() {
  int x = stack.top();
  stack.pop();
  return x;
}

void CPU::eval(std::string program) {
  std::istringstream iss(program);
  std::string word;

  while (shouldRun && iss >> word) {
    if (word == "push") {
      int i;
      iss >> i;
      stack.push(i);
    }
    if (word == "pop") {
      pop();
    }
    if (word == "add") {
      int a = pop();
      int b = pop();
      stack.push(a + b);
    }
    if (word == "neg") {
      stack.push(-pop());
    }
    if (word == "mul") {
      int a = pop();
      int b = pop();
      stack.push(a * b);
    }
    if (word == "divmod") {
      int a = pop();
      int b = pop();
      stack.push(b / a);
      stack.push(b % a);
    }
    if (word == "write") {
      outPort.textBuffer.send(fmt::format("{}", stack.top()));
    }
    if (word == "read") {
      std::optional<std::string> msg;
      do {
        msg = inPort.textBuffer.receive();
      } while (shouldRun && !msg);
      std::istringstream ss(*msg);
      int i;
      ss >> i;
      stack.push(i);
    }
  }

  shouldRun = false;
}

void CPU::run(const std::string& program) {
  shouldRun = false;
  if (evalThread.joinable()) {
    evalThread.join();
  }
  while (!stack.empty()) {
    stack.pop();
  }
  shouldRun = true;
  evalThread = std::thread([this, program] { eval(program); });
}

void CPU::update() {
}

void CPU::render() {
  ImGui::Begin("Programmer");
  static char cbuf[512];
  ImGui::InputTextMultiline("Text", cbuf, 512);

  if (shouldRun) {
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    ImGui::Button("Running...");
    ImGui::PopItemFlag();
    ImGui::SameLine();
    if (ImGui::Button("Stop")) {
      shouldRun = false;
    }
  }
  else {
    if (ImGui::Button("Run")) {
      run(cbuf);
    }
  }
  ImGui::End();
}
