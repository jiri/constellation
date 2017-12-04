#include <Foundation/Components/CPU.hpp>

#include <imgui.h>
#include <imgui_internal.h>

#include <Foundation/Universe.hpp>
#include <Foundation/Systems/Text.hpp>

enum Op : uint8_t {
  /* Flow */
  HALT = 0x00,
  ILLEGAL = 0x01,

  /* Arithmetic */
  ADD = 0x10,
  NEG,
  MUL,
  DIVMOD,

  /* Stack operations */
  PUSH = 0x20,
  POP,
  SWAP,

  /* I/O */
  READ = 0x30,
  WRITE,
};

int16_t CPU::pop() {
  if (stack.empty()) {
    state = ILLEGAL;
    return 0;
  }
  int16_t x = stack.top();
  stack.pop();
  return x;
}

void CPU::push(int16_t x) {
  stack.push(x);
}

void CPU::execute(const CPU::ByteCode& code) {
  size_t pc = 0;

  int16_t a;
  int16_t b;

  shouldRun = true;
  while (shouldRun && state == NORMAL) {
    assert(pc < code.size());

    switch (code[pc++]) {
      case HALT:
        shouldRun = false;
        state = HALTED;
        break;

      /* Arithmetic */
      case ADD:
        b = pop();
        a = pop();
        push(a + b);
        break;

      case NEG:
        a = pop();
        push(-a);
        break;

      case MUL:
        b = pop();
        a = pop();
        push(a * b);
        break;

      case DIVMOD:
        b = pop();
        a = pop();
        push(a / b);
        push(a % b);
        break;

      /* Stack operations */
      case PUSH:
        {
          uint8_t h = code[pc++];
          uint8_t l = code[pc++];
          push(h << 8 | l << 0);
        }
        break;

      case POP:
        pop();
        break;

      case SWAP:
        a = pop();
        b = pop();
        push(a);
        push(b);
        break;

      /* I/O */
      case WRITE:
        this->universe->get<Text::System>().send(&this->out, fmt::format("{}", pop()));
        break;

      case READ:
        {
          std::optional<std::string> msg;
          state = AWAITING_INPUT;
          do {
            msg = this->universe->get<Text::System>().receive(&this->in);
          } while (shouldRun && !msg);
          state = NORMAL;
          std::istringstream(*msg) >> a;
          push(a);
        }
        break;

      /* Error handling */
      case ILLEGAL:
      default:
        state = ILLEGAL;
        shouldRun = false;
        break;
    }
  }

  shouldRun = false;
}

#define HI_BYTE(a) (((a) >> 8) & 0xFF)
#define LO_BYTE(a) ((a) & 0xFF)

CPU::ByteCode CPU::compile(const std::string& program) {
  ByteCode code;

  std::istringstream iss(program);
  std::string word;

  while (iss >> word) {
    /* Flow */
    if (word == "halt") { code.push_back(HALT); }
    /* Arithmetic */
    else if (word == "add")    { code.push_back(ADD);    }
    else if (word == "neg")    { code.push_back(NEG);    }
    else if (word == "mul")    { code.push_back(MUL);    }
    else if (word == "divmod") { code.push_back(DIVMOD); }
    /* Stack operations */
    else if (word == "push")   { int16_t i; iss >> i;
                                 code.push_back(PUSH);
                                 code.push_back(HI_BYTE(i));
                                 code.push_back(LO_BYTE(i)); }
    else if (word == "pop")    { code.push_back(POP);    }
    else if (word == "swap")   { code.push_back(SWAP);   }
    /* I/O */
    else if (word == "read")   { code.push_back(READ);   }
    else if (word == "write")  { code.push_back(WRITE);  }
    else                       { code.push_back(ILLEGAL); }
  }

  code.push_back(HALT);

  return code;
}

void CPU::run(const ByteCode& code) {
  state = NORMAL;
  shouldRun = false;
  if (evalThread.joinable()) {
    evalThread.join();
  }
  while (!stack.empty()) {
    stack.pop();
  }
  evalThread = std::thread([this, code] { execute(code); });
}

void CPU::update() {
}

void CPU::render() {
  ImGui::Begin("Programmer");
  static char cbuf[512];
  ImGui::PushItemWidth(-1);
  ImGui::InputTextMultiline("##code", cbuf, 512);
  ImGui::PopItemWidth();

  if (shouldRun) {
    if (ImGui::Button("Stop")) {
      shouldRun = false;
    }
  }
  else {
    if (ImGui::Button("Run")) {
      run(compile(cbuf));
    }
  }
  ImGui::SameLine();
  switch (state) {
    case NORMAL:
      ImGui::Text("State: NORMAL");
      break;

    case HALTED:
      ImGui::Text("State: HALTED");
      break;

    case ILLEGAL:
      ImGui::PushStyleColor(ImGuiCol_Text, (ImU32)ImColor { 1.0f, 0.0f, 0.0f });
      ImGui::Text("State: ILLEGAL");
      ImGui::PopStyleColor();
      break;

    case AWAITING_INPUT:
      ImGui::Text("State: AWAITING INPUT");
      break;
  }
  ImGui::End();
}
