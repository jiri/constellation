#pragma once

#include <sstream>
#include <string>
#include <tuple>
#include <vector>

template <typename T, typename... Ts>
std::tuple<T, Ts...> __parseTuple(std::vector<std::string>::const_iterator beg,
                                  std::vector<std::string>::const_iterator end) {
  if (beg == end) {
    throw std::runtime_error { "Not enough values to parse" };
  }

  T value;
  std::stringstream stream(*beg);
  stream >> value;

  if constexpr (sizeof...(Ts) == 0) {
    return std::tuple<T>{ value };
  }
  else {
    return std::tuple_cat(std::tuple<T> { value }, __parseTuple<Ts...>(++beg, end));
  }
};

template <typename... Ts>
std::tuple<Ts...> _parseTuple(std::tuple<Ts...>& t, const std::vector<std::string>& ps) {
  if constexpr (sizeof...(Ts) == 0) {
    return {};
  } else {
    return __parseTuple<Ts...>(ps.begin(), ps.end());
  }
};

template <typename T>
T parseTuple(const std::vector<std::string>& ps) {
  T t;
  return _parseTuple(t, ps);
}
