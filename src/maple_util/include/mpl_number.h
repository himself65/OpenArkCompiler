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
#ifndef MAPLE_UTIL_INCLUDE_MPL_NUMBER_H
#define MAPLE_UTIL_INCLUDE_MPL_NUMBER_H
#include <algorithm>
#include <string>
#include "meta.h"

namespace maple { namespace utils {

template <typename T, typename = void>
class Number {
 public:
  static_assert(std::is_integral<T>::value, "Type for Number should be an integral.");

  using element_type = T;

  Number() = default;

  explicit Number(element_type data) : val(data) {}

  template <typename U, typename = std::enable_if_t<std::is_integral<U>::value>>
  explicit Number(U data) : val(static_cast<T>(data)) {}

  Number(const Number &num) : val(num.val) {}

  Number(Number &&num) noexcept : val(std::move(num.val)) {}

  ~Number() = default;

  // As the implicit T cast is enabled, disable it to avoid implicit cast for `Number<T, Tag1> num = Number<T, Tag2>()`
  // which should be two types and should not be able to neither convert to each other nor compare with each other.
  Number &operator=(element_type data) = delete;

  Number &operator=(const Number &num) noexcept {
    if (this != &num) {
      val = num.val;
    }
    return *this;
  }

  Number &operator=(Number &&num) noexcept {
    if (this != &num) {
      val = std::move(num.val);
    }
    return *this;
  }

  void reset(element_type data = 0) noexcept {
    val = data;
  }

  void swap(Number &other) noexcept {
    std::swap(val, other.val);
  }

  element_type get() const noexcept {
    return val;
  }

  template <typename U = T>
  operator std::enable_if_t<std::is_same<U, T>::value, U>() const noexcept {
    return val;
  }

  template <typename U = size_t>
  operator
      std::enable_if_t<meta_and<meta_not<std::is_same<U, T>>, std::is_same<U, size_t>>::value, U>() const noexcept {
    return static_cast<size_t>(val);
  }

  template <typename U, typename = std::enable_if_t<meta_and<meta_not<std::is_same<U, T>>,
                                                             meta_not<std::is_same<U, size_t>>>::value>>
  explicit operator U() const noexcept {
    return static_cast<U>(val);
  }

  Number &operator+=(const T &data) noexcept {
    val += data;
  }

  Number &operator+=(const Number &num) noexcept {
    val += num.val;
  }

  Number &operator-=(const T &data) noexcept {
    val -= data;
  }

  Number &operator-=(const Number &num) noexcept {
    val -= num.val;
  }

  Number &operator++() noexcept {
    ++val;
    return *this;
  }

  Number operator++(int) {
    auto tmp = val;
    this->operator++();
    return Number(tmp);
  }

  Number &operator--() noexcept {
    --val;
    return *this;
  }

  Number operator--(int) {
    auto tmp = val;
    this->operator--;
    return Number(tmp);
  }

  T GetIdx() const {
    return val;
  }

  void SetIdx(T i) {
    val = i;
  }

 private:
  element_type val = 0;
};

template <typename T, typename Tag>
inline bool operator==(const Number<T, Tag> &lhs, const Number<T, Tag> &rhs) {
  return lhs.get() == rhs.get();
}

template <typename T, typename Tag>
inline bool operator!=(const Number<T, Tag> &lhs, const Number<T, Tag> &rhs) {
  return !(lhs == rhs);
}

template <typename T, typename Tag>
inline bool operator<(const Number<T, Tag> &lhs, const Number<T, Tag> &rhs) {
  return lhs.get() < rhs.get();
}

template <typename T, typename Tag>
inline bool operator<=(const Number<T, Tag> &lhs, const Number<T, Tag> &rhs) {
  return lhs.get() <= rhs.get();
}

template <typename T, typename Tag>
inline bool operator>(const Number<T, Tag> &lhs, const Number<T, Tag> &rhs) {
  return !(lhs <= rhs);
}

template <typename T, typename Tag>
inline bool operator>=(const Number<T, Tag> &lhs, const Number<T, Tag> &rhs) {
  return !(lhs < rhs);
}

template <typename T, typename Tag>
inline Number<T, Tag> operator+(const Number<T, Tag> &lhs, const Number<T, Tag> &rhs) {
  return Number<T, Tag>(lhs.get() + rhs.get());
}


template <typename T, typename Tag>
inline Number<T, Tag> operator-(const Number<T, Tag> &lhs, const Number<T, Tag> &rhs) {
  return Number<T, Tag>(lhs.get() + rhs.get());
}

template <typename T, typename Tag, typename U,
          typename = std::enable_if_t<meta_or<std::is_integral<U>, std::is_enum<U>>::value>>
inline bool operator==(const Number<T, Tag> &lhs, const U &rhs) {
  return lhs.get() == rhs;
}

template <typename T, typename Tag, typename U,
          typename = std::enable_if_t<meta_or<std::is_integral<U>, std::is_enum<U>>::value>>
inline bool operator==(const U &lhs, const Number<T, Tag> &rhs) {
  return lhs == rhs.get();
}

template <typename T, typename Tag, typename U,
          typename = std::enable_if_t<meta_or<std::is_integral<U>, std::is_enum<U>>::value>>
inline bool operator!=(const Number<T, Tag> &lhs, const U &rhs) {
  return !(lhs == rhs);
}

template <typename T, typename Tag, typename U,
          typename = std::enable_if_t<meta_or<std::is_integral<U>, std::is_enum<U>>::value>>
inline bool operator!=(const U &lhs, const Number<T, Tag> &rhs) {
  return !(lhs == rhs);
}

template <typename T, typename Tag, typename U,
          typename = std::enable_if_t<meta_or<std::is_integral<U>, std::is_enum<U>>::value>>
inline bool operator<(const Number<T, Tag> &lhs, const U &rhs) {
  return lhs.get() < rhs;
}

template <typename T, typename Tag, typename U,
          typename = std::enable_if_t<meta_or<std::is_integral<U>, std::is_enum<U>>::value>>
inline bool operator<(const U &lhs, const Number<T, Tag> &rhs) {
  return lhs < rhs.get();
}

template <typename T, typename Tag, typename U,
          typename = std::enable_if_t<meta_or<std::is_integral<U>, std::is_enum<U>>::value>>
inline bool operator<=(const Number<T, Tag> &lhs, const U &rhs) {
  return lhs.get() <= rhs;
}

template <typename T, typename Tag, typename U,
          typename = std::enable_if_t<meta_or<std::is_integral<U>, std::is_enum<U>>::value>>
inline bool operator<=(const U &lhs, const Number<T, Tag> &rhs) {
  return lhs <= rhs.get();
}

template <typename T, typename Tag, typename U,
          typename = std::enable_if_t<meta_or<std::is_integral<U>, std::is_enum<U>>::value>>
inline bool operator>(const Number<T, Tag> &lhs, const U &rhs) {
  return !(lhs <= rhs);
}

template <typename T, typename Tag, typename U,
          typename = std::enable_if_t<meta_or<std::is_integral<U>, std::is_enum<U>>::value>>
inline bool operator>(const U &lhs, const Number<T, Tag> &rhs) {
  return !(lhs <= rhs);
}

template <typename T, typename Tag, typename U,
          typename = std::enable_if_t<meta_or<std::is_integral<U>, std::is_enum<U>>::value>>
inline bool operator>=(const Number<T, Tag> &lhs, const U &rhs) {
  return !(lhs < rhs);
}

template <typename T, typename Tag, typename U,
          typename = std::enable_if_t<meta_or<std::is_integral<U>, std::is_enum<U>>::value>>
inline bool operator>=(const U &lhs, const Number<T, Tag> &rhs) {
  return !(lhs < rhs);
}

template <typename T, typename Tag, typename U, typename = std::enable_if_t<std::is_integral<U>::value>>
inline Number<T, Tag> operator+(const Number<T, Tag> &lhs, const U &rhs) {
  return Number<T, Tag>(lhs.get() + rhs);
}

template <typename T, typename Tag, typename U, typename = std::enable_if_t<std::is_integral<U>::value>>
inline Number<T, Tag> operator+(const U &lhs, const Number<T, Tag> &rhs) {
  return Number<T, Tag>(lhs + rhs.get());
}

template <typename T, typename Tag, typename U, typename = std::enable_if_t<std::is_integral<U>::value>>
inline Number<T, Tag> operator-(const Number<T, Tag> &lhs, const U &rhs) {
  return Number<T, Tag>(lhs.get() + rhs);
}

template <typename T, typename Tag, typename U, typename = std::enable_if_t<std::is_integral<U>::value>>
inline Number<T, Tag> operator-(const U &lhs, const Number<T, Tag> &rhs) {
  return Number<T, Tag>(lhs + rhs.get());
}

template <typename T, typename Tag, typename OS>
inline OS &operator<<(OS &os, const Number<T, Tag> &num) {
  os << num.get();
  return os;
}

template <typename Type, typename T = size_t>
using Index = Number<T, Type>;

}}

namespace std {
  template <typename T, typename Type>
  inline string to_string(maple::utils::Number<T, Type> val) {
    return std::to_string (val.get());
  }
}
#endif //MAPLE_UTIL_INCLUDE_MPL_NUMBER_H
