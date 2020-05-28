/*
 * Copyright (c) [2019-2020] Huawei Technologies Co.,Ltd.All rights reserved.
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
#ifndef MAPLE_UTIL_PTR_H
#define MAPLE_UTIL_PTR_H
#include <functional>

namespace maple { namespace utils {
template<typename T>
using PtrCheckerType = void (*)(const T*);

template<typename T>
inline constexpr void CheckNothing(const T*) {}

template<typename T, PtrCheckerType<T> Check = CheckNothing<T>>
class Ptr {
 public:
  using Pointer = T*;
  using element_type = T;

  constexpr Ptr() noexcept
      : pointer(nullptr) {
    Check(nullptr);
  }

  constexpr explicit Ptr(std::nullptr_t) noexcept
      : pointer(nullptr) {
    Check(nullptr);
  }

  explicit Ptr(Pointer ptr)
      : pointer(ptr) {
    Check(ptr);
  }

  Ptr(Pointer ref, PtrCheckerType<T> checker)
      : pointer(ref) {
    checker(pointer);
  }

  explicit Ptr(T &ref)
      : pointer(&ref) {
    Check(pointer);
  }

  Ptr(T &ref, PtrCheckerType<T> checker)
      : pointer(&ref) {
    checker(pointer);
  }

  explicit Ptr(T &&ref) = delete;

  template<typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  explicit Ptr(U *ptr)
      : pointer(ptr) {
    Check(pointer);
  }

  template<typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  explicit Ptr(U &ref)
      : pointer(&ref) {
    Check(pointer);
  }

  template<typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  Ptr(U &ref, PtrCheckerType<T> checker)
      : pointer(&ref) {
    checker(pointer);
  }

  template<typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  explicit Ptr(U &&ref) = delete;

  explicit Ptr(const Ptr &other)
      : pointer(other.get()) {}

  explicit Ptr(Ptr &&other) noexcept
      : pointer(other.get()) {}

  template<typename U, PtrCheckerType<U> CheckU, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  explicit Ptr(const Ptr<U, CheckU> &other)
      : pointer(other.get()) {
    Check(pointer);
  }

  template<typename U, PtrCheckerType<U> CheckU, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  Ptr(const Ptr<U, CheckU> &other, PtrCheckerType<T> checker)
      : pointer(other.get()) {
    checker(pointer);
  }

  template<typename U, PtrCheckerType<U> CheckU, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  explicit Ptr(Ptr<U, CheckU> &&other)
      : pointer(other.get()) {
    Check(pointer);
  }

  template<typename U, PtrCheckerType<U> CheckU, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  Ptr(Ptr<U, CheckU> &&other, PtrCheckerType<T> checker)
      : pointer(other.get()) {
    checker(pointer);
  }

  ~Ptr() = default;

  Ptr &operator=(Pointer ptr) noexcept {
    Check(ptr);
    pointer = ptr;
    return *this;
  }

  Ptr &operator=(std::nullptr_t) noexcept {
    Check(nullptr);
    reset();
    return *this;
  }

  Ptr &operator=(const Ptr &ptr) noexcept {
    if (this != &ptr) {
      pointer = ptr.pointer;
    }
    return *this;
  }

  Ptr &operator=(Ptr &&ptr) noexcept {
    if (this != &ptr) {
      pointer = std::move(ptr.pointer);
    }
    return *this;
  }

  template<typename U, PtrCheckerType<U> CheckU, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  Ptr &operator=(const Ptr<U, CheckU> &ptr) noexcept {
    if (this->get() != ptr.get()) {
      Check(ptr.get());
      pointer = ptr.get();
    }
    return *this;
  }

  template<typename U, PtrCheckerType<U> CheckU, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  Ptr &operator=(Ptr<U, CheckU> &&ptr) noexcept {
    if (this->get() != ptr.get()) {
      Check(ptr.get());
      pointer = std::move(ptr.get());
    }
    return *this;
  }

  Pointer release() noexcept = delete;

  void reset(Pointer ptr = Pointer()) noexcept {
    Check(ptr);
    pointer = ptr;
  }

  void swap(Ptr &other) noexcept {
    std::swap(pointer, other.pointer);
  }

  T *get() const noexcept {
    return pointer;
  }

  explicit operator bool() const noexcept {
    return get() != nullptr;
  }

  T &operator*() const {
    return *get();
  }

  T *operator->() const noexcept {
    return get();
  }

  T &operator[](size_t i) const = delete;

  Ptr &operator++() = delete;

  const Ptr operator++(int) = delete;

  Ptr &operator--() = delete;

  const Ptr operator--(int) = delete;

  Ptr &operator+=(std::ptrdiff_t) = delete;

  Ptr &operator-=(std::ptrdiff_t) = delete;

 private:
  T *pointer;
};

template<typename T, PtrCheckerType<T> CheckT, typename U, PtrCheckerType<U> CheckU>
inline bool operator==(const Ptr<T, CheckT> &lhs, const Ptr<U, CheckU> &rhs) {
  return lhs.get() == rhs.get();
}

template<typename T, PtrCheckerType<T> CheckT, typename U, PtrCheckerType<U> CheckU>
inline bool operator!=(const Ptr<T, CheckT> &lhs, const Ptr<U, CheckU> &rhs) {
  return !(lhs == rhs);
}

template<typename T, PtrCheckerType<T> CheckT, typename U, PtrCheckerType<U> CheckU>
inline bool operator<(const Ptr<T, CheckT> &lhs, const Ptr<U, CheckU> &rhs) {
  return lhs.get() < rhs.get();
}

template<typename T, PtrCheckerType<T> CheckT, typename U, PtrCheckerType<U> CheckU>
inline bool operator<=(const Ptr<T, CheckT> &lhs, const Ptr<U, CheckU> &rhs) {
  return lhs.get() <= rhs.get();
}

template<typename T, PtrCheckerType<T> CheckT, typename U, PtrCheckerType<U> CheckU>
inline bool operator>(const Ptr<T, CheckT> &lhs, const Ptr<U, CheckU> &rhs) {
  return !(lhs <= rhs);
}

template<typename T, PtrCheckerType<T> CheckT, typename U, PtrCheckerType<U> CheckU>
inline bool operator>=(const Ptr<T, CheckT> &lhs, const Ptr<U, CheckU> &rhs) {
  return !(lhs < rhs);
}

template<typename T, PtrCheckerType<T> CheckT>
inline bool operator==(const Ptr<T, CheckT> &lhs, std::nullptr_t) {
  return !static_cast<bool>(lhs);
}

template<typename T, PtrCheckerType<T> CheckT>
inline bool operator==(std::nullptr_t, const Ptr<T, CheckT> &rhs) {
  return !static_cast<bool>(rhs);
}

template<typename T, PtrCheckerType<T> CheckT>
inline bool operator!=(const Ptr<T, CheckT> &lhs, std::nullptr_t) {
  return static_cast<bool>(lhs);
}

template<typename T, PtrCheckerType<T> CheckT>
inline bool operator!=(std::nullptr_t, const Ptr<T, CheckT> &rhs) {
  return static_cast<bool>(rhs);
}

template<typename T, PtrCheckerType<T> CheckT>
inline bool operator<(const Ptr<T, CheckT> &lhs, std::nullptr_t) {
  return std::less<typename Ptr<T, CheckT>::Pointer>()(lhs.get(), nullptr);
}

template<typename T, PtrCheckerType<T> CheckT>
inline bool operator<(std::nullptr_t, const Ptr<T, CheckT> &rhs) {
  return std::less<typename Ptr<T, CheckT>::Pointer>()(nullptr, rhs.get());
}

template<typename T, PtrCheckerType<T> CheckT>
inline bool operator<=(const Ptr<T, CheckT> &lhs, std::nullptr_t) {
  return !(nullptr < lhs);
}

template<typename T, PtrCheckerType<T> CheckT>
inline bool operator<=(std::nullptr_t, const Ptr<T, CheckT> &rhs) {
  return !(rhs < nullptr);
}

template<typename T, PtrCheckerType<T> CheckT>
inline bool operator>(const Ptr<T, CheckT> &lhs, std::nullptr_t) {
  return !(lhs <= nullptr);
}

template<typename T, PtrCheckerType<T> CheckT>
inline bool operator>(std::nullptr_t, const Ptr<T, CheckT> &rhs) {
  return !(nullptr <= rhs);
}

template<typename T, PtrCheckerType<T> CheckT>
inline bool operator>=(const Ptr<T, CheckT> &lhs, std::nullptr_t) {
  return !(lhs < nullptr);
}

template<typename T, PtrCheckerType<T> CheckT>
inline bool operator>=(std::nullptr_t, const Ptr<T, CheckT> &rhs) {
  return !(nullptr < rhs);
}
}}

#endif //DIY_CPLUSPLUS_SAFE_PTR_H
