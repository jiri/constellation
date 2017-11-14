#include <Foundation/Components/CPU.hpp>

#include <imgui.h>

int CPU::pop() {
  int x = stack.top();
  stack.pop();
  return x;
}

void CPU::run(const std::string& program) {
  std::istringstream iss(program);
  std::string word;

  while (iss >> word) {
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
      port.textBuffer.send(fmt::format("{}", stack.top()));
    }
  }
}

void CPU::update() {
}

void CPU::render() {
  ImGui::Begin("Terminal");
  char cbuf[512];
  ImGui::InputTextMultiline(nullptr, cbuf, 512);
  if (ImGui::Button("Run")) {
    while (stack.size())
      stack.pop();
    run(cbuf);
  }
  ImGui::End();
}
