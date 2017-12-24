#pragma once

template <typename T>
struct function_traits
  : public function_traits<decltype(&T::operator())>
{ };

template <typename C, typename R, typename... Ps>
struct function_traits<R(C::*)(Ps...) const> {
  using result_type = R;
  using argument_tuple_type = std::tuple<Ps...>;
  static constexpr size_t arity = sizeof...(Ps);
};
