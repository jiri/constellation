#pragma once

#include <Foundation/Infrastructures/Infrastructure.hpp>
#include <Util/Filesystem.hpp>

class Manual : public Infrastructure {
public:
  using Infrastructure::Infrastructure;

  void update() override;

  void save(const fs::path& path);
  void load(const fs::path& path);

private:
  char buf[256] {};
};


