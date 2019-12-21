/*
 * Copyright (c) [2019] Huawei Technologies Co.,Ltd.All rights reserved.
 *
 * OpenArkCompiler is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *
 *     http://license.coscl.org.cn/MulanPSL
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR
 * FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v1 for more details.
 */
#ifndef MAPLE_UTIL_INCLUDE_META_H
#define MAPLE_UTIL_INCLUDE_META_H
#include <type_traits>

namespace maple { namespace utils {
template <typename T, typename U>
struct meta_and
 : public std::conditional_t<T::value, U, T> {};

template <typename T, typename U>
struct meta_or
  : public std::conditional_t<T::value, T, U> {};

template <typename T>
struct meta_not
  : public std::integral_constant<bool, !bool(T::value)> {};

template <typename ...>
struct is_signed;

template <>
struct is_signed<>
  : public std::true_type {};

template <typename T>
struct is_signed<T>
  : public std::is_signed<T> {};

template <typename T, typename U>
struct is_signed<T, U>
  : public meta_and<std::is_signed<T>, std::is_signed<U>> {};

template <typename ...T>
constexpr bool is_signed_v = is_signed<T...>::value;

template <typename ...>
struct is_unsigned;

template <>
struct is_unsigned<>
  : public std::true_type {};

template <typename T>
struct is_unsigned<T>
  : public std::is_unsigned<T> {};

template <typename T, typename U>
struct is_unsigned<T, U>
  : public meta_and<std::is_unsigned<T>, std::is_unsigned<U>> {};

template <typename ...T>
constexpr bool is_unsigned_v = is_unsigned<T...>::value;

template <typename T, typename U>
struct is_same_sign
  : public meta_or<is_signed<T, U>, is_unsigned<T, U>> {};

template <typename T, typename U>
struct is_diff_sign
  : public meta_not<is_same_sign<T, U>> {};
}}
#endif //MAPLE_UTIL_INCLUDE_META_H
