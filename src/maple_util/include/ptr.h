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
  using pointer = T*;
  using element_type = T;

  constexpr Ptr() noexcept
      : ptr_(nullptr) {
    Check(nullptr);
  }

  constexpr explicit Ptr(std::nullptr_t) noexcept
      : ptr_(nullptr) {
    Check(nullptr);
  }

  explicit Ptr(pointer ptr)
      : ptr_(ptr) {
    Check(ptr);
  }

  Ptr(pointer ref, PtrCheckerType<T> checker)
      : ptr_(ref) {
    checker(ptr_);
  }

  explicit Ptr(T &ref)
      : ptr_(&ref) {
    Check(ptr_);
  }

  Ptr(T &ref, PtrCheckerType<T> checker)
      : ptr_(&ref) {
    checker(ptr_);
  }

  Ptr(T &&ref) = delete;

  template<typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  explicit Ptr(U *ptr)
      : ptr_(ptr) {
    Check(ptr_);
  }

  template<typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  explicit Ptr(U &ref)
      : ptr_(&ref) {
    Check(ptr_);
  }

  template<typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  Ptr(U &ref, PtrCheckerType<T> checker)
      : ptr_(&ref) {
    checker(ptr_);
  }

  template<typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  Ptr(U &&ref) = delete;

  Ptr(const Ptr &other)
      : ptr_(other.get()) {}

  Ptr(Ptr &&other) noexcept
      : ptr_(other.get()) {}

  template<typename U, PtrCheckerType<U> CheckU, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  explicit Ptr(const Ptr<U, CheckU> &other)
      : ptr_(other.get()) {
    Check(ptr_);
  }

  template<typename U, PtrCheckerType<U> CheckU, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  Ptr(const Ptr<U, CheckU> &other, PtrCheckerType<T> checker)
      : ptr_(other.get()) {
    checker(ptr_);
  }

  template<typename U, PtrCheckerType<U> CheckU, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  explicit Ptr(Ptr<U, CheckU> &&other)
      : ptr_(other.get()) {
    Check(ptr_);
  }

  template<typename U, PtrCheckerType<U> CheckU, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  Ptr(Ptr<U, CheckU> &&other, PtrCheckerType<T> checker)
      : ptr_(other.get()) {
    checker(ptr_);
  }

  ~Ptr() = default;

  Ptr &operator=(pointer ptr) noexcept {
    Check(ptr);
    ptr_ = ptr;
    return *this;
  }

  Ptr &operator=(std::nullptr_t) noexcept {
    Check(nullptr);
    reset();
    return *this;
  }

  Ptr &operator=(const Ptr &ptr) noexcept {
    if (this != &ptr) {
      ptr_ = ptr.ptr_;
    }
    return *this;
  }

  Ptr &operator=(Ptr &&ptr) noexcept {
    if (this != &ptr) {
      ptr_ = std::move(ptr.ptr_);
    }
    return *this;
  }

  template<typename U, PtrCheckerType<U> CheckU, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  Ptr &operator=(const Ptr<U, CheckU> &ptr) noexcept {
    if (this->get() != ptr.get()) {
      Check(ptr.get());
      ptr_ = ptr.get();
    }
    return *this;
  }

  template<typename U, PtrCheckerType<U> CheckU, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  Ptr &operator=(Ptr<U, CheckU> &&ptr) noexcept {
    if (this->get() != ptr.get()) {
      Check(ptr.get());
      ptr_ = std::move(ptr.get());
    }
    return *this;
  }

  pointer release() noexcept = delete;

  void reset(pointer ptr = pointer()) noexcept {
    Check(ptr);
    ptr_ = ptr;
  }

  void swap(Ptr &other) noexcept {
    std::swap(ptr_, other.ptr_);
  }

  T *get() const noexcept {
    return ptr_;
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
  T *ptr_;
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
  return std::less<typename Ptr<T, CheckT>::pointer>()(lhs.get(), nullptr);
}

template<typename T, PtrCheckerType<T> CheckT>
inline bool operator<(std::nullptr_t, const Ptr<T, CheckT> &rhs) {
  return std::less<typename Ptr<T, CheckT>::pointer>()(nullptr, rhs.get());
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
