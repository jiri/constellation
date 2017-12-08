#pragma once


#include <Foundation/Infrastructures/Infrastructure.hpp>

class Debug : public Infrastructure {
public:
  using Infrastructure::Infrastructure;

  void update() override;

private:
  char buf[256] {};
};


