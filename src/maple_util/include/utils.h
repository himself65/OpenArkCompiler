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
#ifndef MAPLE_UTIL_INCLUDE_UTILS_H
#define MAPLE_UTIL_INCLUDE_UTILS_H
#include <type_traits>
#include "mpl_logging.h"

namespace maple { namespace utils {

// Operations on char
inline constexpr bool IsDigit(char c) {
  return (c >= '0' && c <= '9');
}

inline constexpr bool IsLower(char c) {
  return (c >= 'a' && c <= 'z');
}

inline constexpr bool IsUpper(char c) {
  return (c >= 'A' && c <= 'Z');
}

inline constexpr bool IsAlpha(char c) {
  return (IsLower(c) || IsUpper(c));
}

inline constexpr bool IsAlnum(char c) {
  return (IsAlpha(c) || IsDigit(c));
}

namespace __ToDigitImpl {

template <typename T>
struct TypeMax {};

template <>
struct TypeMax<uint8_t> {
  enum {value = UINT8_MAX};
};

template <>
struct TypeMax<int32_t> {
  enum {value = INT32_MAX};
};

template <uint8_t Scale, typename T>
struct ToDigitImpl {};

template <typename T>
struct ToDigitImpl<10, T> {
  static T DoIt(char c) {
    if (utils::IsDigit(c)) {
      return c - '0';
    }
    return TypeMax<T>::value;
  }
};

template <typename T>
struct ToDigitImpl<8, T> {
  static T DoIt(char c) {
    if (c >= '0' && c < '8') {
      return c - '0';
    }
    return TypeMax<T>::value;
  }
};

template <typename T>
struct ToDigitImpl<16, T> {
  static T DoIt(char c) {
    if (utils::IsDigit(c)) {
      return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
      return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
      return c - 'A' + 10;
    }
    return TypeMax<T>::value;
  }
};
}

template <uint8_t Scale = 10, typename T = uint8_t>
inline constexpr T ToDigit(char c) {
  return __ToDigitImpl::ToDigitImpl<Scale, T>::DoIt(c);
}

// Operations on pointer
template <typename T>
inline T &ToRef(T *ptr) {
  CHECK_NULL_FATAL(ptr);
  return *ptr;
}

template <typename T, typename = decltype(&T::get)>
inline typename T::element_type &ToRef(T &ptr) {
  return ToRef(ptr.get());
}

}} // namespace maple::utils
#endif  // MAPLE_UTIL_INCLUDE_UTILS_H
