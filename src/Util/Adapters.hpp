#pragma once

#include <iterator>

namespace adapters {
  template <typename T>
  class reverse {
  public:
    explicit reverse(T& iter)
      : iterable { iter }
    { }

    auto begin() {
      return std::rbegin(iterable);
    }

    auto end() {
      return std::rend(iterable);
    }

  private:
    T& iterable;
  };
}
