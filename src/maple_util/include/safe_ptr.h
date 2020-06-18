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
#ifndef MAPLE_UTIL_SAFE_PTR_H
#define MAPLE_UTIL_SAFE_PTR_H
#include <mpl_logging.h>
#include "ptr.h"

namespace maple {
namespace utils {
template<typename T>
inline void AssertNotNull(const T *ptr) {
  CHECK_FATAL(ptr != nullptr, "nullptr was assigned to SafePtr.");
}

template<typename T>
class SafePtr {
  using Base = Ptr<T, AssertNotNull<T>>;
 public:
  using pointer = typename Base::Pointer;
  using ElementType = typename Base::element_type;

  constexpr SafePtr() noexcept = delete;

  constexpr SafePtr(std::nullptr_t) noexcept = delete;

  SafePtr(pointer ptr) : base(ptr) {}

  SafePtr(T &ref) : base(ref, CheckNothing<T>) {}

  SafePtr(T &&ref) = delete;

  template<typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  SafePtr(U *ptr) : base(ptr) {}

  template<typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  SafePtr(U &ref) : base(ref, CheckNothing<T>) {}

  template<typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  SafePtr(U &&ref) = delete;

  SafePtr(const SafePtr &other) : base(other.base) {}

  SafePtr(SafePtr &&other) noexcept : base(std::move(other.base)) {}

  template<typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  explicit SafePtr(const SafePtr<U> &other) : base(other.get(), CheckNothing<T>) {}

  template<typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  explicit SafePtr(SafePtr<U> &&other) : base(other.get(), CheckNothing<T>) {}

  ~SafePtr() = default;

  SafePtr &operator=(pointer ptr) noexcept {
    base = ptr;
    return *this;
  }

  SafePtr &operator=(std::nullptr_t) noexcept = delete;

  SafePtr &operator=(const SafePtr &ptr) noexcept {
    base = ptr.base;
    return *this;
  }

  SafePtr &operator=(SafePtr &&ptr) noexcept {
    base = std::move(ptr.base);
    return *this;
  }

  template<typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  SafePtr &operator=(const SafePtr<U> &ptr) noexcept {
    base = Base(static_cast<T*>(ptr.get()), CheckNothing<T>);
    return *this;
  }

  template<typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
  SafePtr &operator=(SafePtr<U> &&ptr) noexcept {
    base = Base(static_cast<T*>(ptr.get()), CheckNothing<T>);
    return *this;
  }

  pointer release() noexcept = delete;

  void reset(std::nullptr_t) noexcept = delete;

  void reset(pointer ptr) noexcept {
    AssertNotNull(ptr);
    base.reset(ptr);
  }

  void swap(SafePtr &other) noexcept {
    std::swap(base, other.base);
  }

  T *get() const noexcept {
    return base.get();
  }

  explicit operator bool() const noexcept = delete;

  T &operator*() const {
    return *get();
  }

  T *operator->() const noexcept {
    return get();
  }

  SafePtr &operator++() = delete;

  const SafePtr operator++(int) = delete;

  SafePtr &operator--() = delete;

  const SafePtr operator--(int) = delete;

  SafePtr &operator+=(std::ptrdiff_t) = delete;

  SafePtr &operator-=(std::ptrdiff_t) = delete;

 private:
  Base base;
};

template<typename T, typename U>
inline bool operator==(const SafePtr<T> &lhs, const SafePtr<U> &rhs) {
  return lhs.get() == rhs.get();
}

template<typename T, typename U>
inline bool operator!=(const SafePtr<T> &lhs, const SafePtr<U> &rhs) {
  return !(lhs == rhs);
}

template<typename T, typename U>
inline bool operator<(const SafePtr<T> &lhs, const SafePtr<U> &rhs) = delete;

template<typename T, typename U>
inline bool operator<=(const SafePtr<T> &lhs, const SafePtr<U> &rhs) = delete;

template<typename T, typename U>
inline bool operator>(const SafePtr<T> &lhs, const SafePtr<U> &rhs) = delete;

template<typename T, typename U>
inline bool operator>=(const SafePtr<T> &lhs, const SafePtr<U> &rhs) = delete;

template<typename T>
inline bool operator==(const SafePtr<T> &lhs, std::nullptr_t) = delete;

template<typename T>
inline bool operator==(std::nullptr_t, const SafePtr<T> &rhs) = delete;

template<typename T>
inline bool operator!=(const SafePtr<T> &lhs, std::nullptr_t) = delete;

template<typename T>
inline bool operator!=(std::nullptr_t, const SafePtr<T> &rhs) = delete;

template<typename T>
inline bool operator<(const SafePtr<T> &lhs, std::nullptr_t) = delete;

template<typename T>
inline bool operator<(std::nullptr_t, const SafePtr<T> &rhs) = delete;

template<typename T>
inline bool operator<=(const SafePtr<T> &lhs, std::nullptr_t) = delete;

template<typename T>
inline bool operator<=(std::nullptr_t, const SafePtr<T> &rhs) = delete;

template<typename T>
inline bool operator>(const SafePtr<T> &lhs, std::nullptr_t) = delete;

template<typename T>
inline bool operator>(std::nullptr_t, const SafePtr<T> &rhs) = delete;

template<typename T>
inline bool operator>=(const SafePtr<T> &lhs, std::nullptr_t) = delete;

template<typename T>
inline bool operator>=(std::nullptr_t, const SafePtr<T> &rhs) = delete;

template<typename T>
inline T &ToRef(SafePtr<T> ptr) {
  return *ptr;
}
}
}

namespace std {
template<class T>
struct hash<maple::utils::SafePtr<T>> {
  std::size_t operator()(const maple::utils::SafePtr<T> &safePtr) const {
    return hash<typename maple::utils::SafePtr<T>::pointer>()(safePtr.get());
  }
};
} // namespace std
#endif //DIY_CPLUSPLUS_SAFE_PTR_H
