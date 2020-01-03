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
#include <limits>
#include "mpl_logging.h"

namespace maple { namespace utils {
// Operations on char
constexpr bool IsDigit(char c) {
  return (c >= '0' && c <= '9');
}

constexpr bool IsLower(char c) {
  return (c >= 'a' && c <= 'z');
}

constexpr bool IsUpper(char c) {
  return (c >= 'A' && c <= 'Z');
}

constexpr bool IsAlpha(char c) {
  return (IsLower(c) || IsUpper(c));
}

constexpr bool IsAlnum(char c) {
  return (IsAlpha(c) || IsDigit(c));
}

namespace __ToDigitImpl {
template <uint8_t Scale, typename T>
struct ToDigitImpl {};

template <typename T>
struct ToDigitImpl<10, T> {
  static T DoIt(char c) {
    if (utils::IsDigit(c)) {
      return c - '0';
    }
    return std::numeric_limits<T>::max();
  }
};

template <typename T>
struct ToDigitImpl<8, T> {
  static T DoIt(char c) {
    if (c >= '0' && c < '8') {
      return c - '0';
    }
    return std::numeric_limits<T>::max();
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
    return std::numeric_limits<T>::max();
  }
};
}

template <uint8_t Scale = 10, typename T = uint8_t>
constexpr T ToDigit(char c) {
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

template <size_t pos, typename = std::enable_if_t<pos < 32>>
struct bit_field {
  enum {value = 1U << pos};
};

template <size_t pos>
constexpr uint32_t bit_field_v = bit_field<pos>::value;

template <size_t pos, typename = std::enable_if_t<pos < 64>>
struct lbit_field {
  enum {value = 1UL << pos};
};

template <size_t pos>
constexpr uint64_t lbit_field_v = bit_field<pos>::value;

}} // namespace maple::utils
#endif  // MAPLE_UTIL_INCLUDE_UTILS_H
