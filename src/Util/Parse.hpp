#pragma once

#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include <fmt/format.h>

template <typename T, typename... Ts>
std::tuple<T, Ts...> __parseTuple(size_t ix,
                                  std::vector<std::string>::const_iterator beg,
                                  std::vector<std::string>::const_iterator end) {
  if (beg == end) {
    throw std::runtime_error { "Not enough arguments" };
  }

  T value;
  std::stringstream stream(*beg);
  stream >> value;

  if (!stream) {
    throw std::runtime_error { fmt::format("Could not parse argument {}", ix) };
  }

  if constexpr (sizeof...(Ts) == 0) {
    if (++beg != end) {
      throw std::runtime_error { "Too many arguments" };
    }
    return std::tuple<T>{ value };
  }
  else {
    return std::tuple_cat(std::tuple<T> { value }, __parseTuple<Ts...>(ix + 1, ++beg, end));
  }
};

template <typename... Ts>
std::tuple<Ts...> _parseTuple(std::tuple<Ts...>& t, const std::vector<std::string>& ps) {
  if constexpr (sizeof...(Ts) == 0) {
    return {};
  } else {
    return __parseTuple<Ts...>(1, ps.begin(), ps.end());
  }
};

template <typename T>
T parseTuple(const std::vector<std::string>& ps) {
  T t;
  return _parseTuple(t, ps);
}

template <typename T, typename... Ts>
std::string __tupleToString() {
  if constexpr (sizeof...(Ts) == 0) {
    return fmt::format("<{}>", typeid(T).name());
  }
  else {
    return fmt::format("<{}> {}", typeid(T).name(), __tupleToString<Ts...>());
  }
};

template <typename... Ts>
std::string _tupleToString(const std::tuple<Ts...>& t) {
  if constexpr (sizeof...(Ts) == 0) {
    return "";
  }
  else {
    return __tupleToString<Ts...>();
  }
};

template <typename T>
std::string tupleToString() {
  return _tupleToString(T{});
}
