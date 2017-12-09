#pragma once

#include <sstream>
#include <string>
#include <tuple>
#include <vector>

template <typename T, typename... Ts>
std::tuple<T, Ts...> parseTuple(std::vector<std::string>::const_iterator beg,
                                std::vector<std::string>::const_iterator end) {
  if (beg == end) {
    throw std::runtime_error { "Not enough values to parse" };
  }

  T t;
  std::stringstream stream(*beg);
  stream >> t;

  if constexpr (sizeof...(Ts) == 0) {
    return std::tuple<T>{ t };
  }
  else {
    return std::tuple_cat(std::tuple<T> { t }, parseTuple<Ts...>(++beg, end));
  }
}

template <typename T, typename... Ts>
std::tuple<T, Ts...> parseTuple(const std::vector<std::string>& ps) {
  return parseTuple<T, Ts...>(ps.begin(), ps.end());
};
