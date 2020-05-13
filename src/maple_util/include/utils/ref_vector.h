/*
 * Copyright (c) [2020] Huawei Technologies Co.,Ltd.All rights reserved.
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
#ifndef MAPLE_UTIL_INCLUDE_REF_VECTOR_H
#define MAPLE_UTIL_INCLUDE_REF_VECTOR_H
#include <vector>
#include "iterator.h"

// The `utils::ref_vector<T>` is a decorator of `std::vector<T*>` which won't destory any `T`s and just refer to them.
//
// It is designed mainly to solve the problem that only the owner who is very familiar can tell whether the `T*` can be
// nullptr. As time goes by, the others, even the owner may forget it. Or with more coding base on it, some will start
// to check nullptr, it will make the code more confusion and hard to maintain.
// Based on that, the ref_vector picks up the meaning of keeping objects' reference, it won't destory any of its
// elements or require to check null(Never to be nullptr).
namespace maple { namespace utils {
class ref_vector_tag;
template <typename Iterator>
struct mpl_iterator_traits<Iterator, ref_vector_tag> {
  using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
  using value_type = std::remove_pointer_t<typename std::iterator_traits<Iterator>::value_type>;
  using difference_type = typename std::iterator_traits<Iterator>::difference_type;
  using pointer = value_type*;
  using reference = value_type&;

  static constexpr reference operator_dereference(Iterator iter) {
    return *(*iter);
  };

  static constexpr Iterator operator_arrow(Iterator iter) {
    return *iter;
  }

  static reference operator_bracket(Iterator iter, difference_type n) {
    return *(iter[n]);
  }
};

template <typename T, typename Alloc = std::allocator<T>>
class ref_vector {
  using base_vector = std::vector<T*, Alloc>;
 public:
  using value_type = T;
  using allocator_type = Alloc;
  using size_type = typename base_vector::size_type;
  using difference_type = typename base_vector::difference_type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = mpl_iterator<typename base_vector::pointer, ref_vector_tag>;
  using const_iterator = mpl_iterator<typename base_vector::const_pointer, ref_vector_tag>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  ref_vector() noexcept = default;

  explicit ref_vector(const allocator_type &alloc) noexcept
      : base(alloc) {}

  // There shouldn't be any situation that initialize this with all the same reference. Maybe an initial small size.
  explicit ref_vector(size_type count, const value_type &value,
                      const allocator_type &alloc = allocator_type()) = delete;
  explicit ref_vector(size_type count, const allocator_type &alloc = allocator_type()) = delete;

  // The pointer is stored in the base, and the Iter owns objects instead of their pointers.
  template <typename Iter>
  ref_vector(Iter first, Iter last, const allocator_type &alloc = allocator_type()) = delete;

  ref_vector(const ref_vector &other)
      : base(other.base) {}

  ref_vector(const ref_vector &other, const allocator_type &alloc)
      : base(other.base, alloc) {}

  ref_vector(ref_vector &&other) noexcept
      : base(std::move(other)) {}

  ref_vector(ref_vector &&other, const allocator_type &alloc) noexcept
      : base(std::move(other), alloc) {}

  ref_vector(std::initializer_list<value_type> init, const allocator_type &alloc = allocator_type()) = delete;

  ~ref_vector() noexcept = default;

  ref_vector &operator=(const ref_vector &other) {
    base = other.base;
    return *this;
  }

  ref_vector &operator=(ref_vector &&other) noexcept {
    base = std::move(other);
  }

  ref_vector &operator=(std::initializer_list<value_type> init) = delete;

  void assign(size_type count, const value_type &value) = delete;

  // The pointer is stored in the base, and the Iter owns objects instead of their pointers.
  template <typename Iter>
  void assign(Iter first, Iter last) = delete;

  void assign(std::initializer_list<value_type> init) = delete;

  allocator_type get_allocator() const {
    return base.get_allocator();
  }

  iterator begin() noexcept {
    return iterator(base.begin());
  }

  const_iterator begin() const noexcept {
    return const_iterator(base.begin());
  }

  iterator end() noexcept {
    return iterator(base.end());
  }

  const_iterator end() const noexcept {
    return const_iterator(base.end());
  }

  reverse_iterator rbegin() noexcept {
    return reverse_iterator(base.rbegin());
  }

  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(base.rbegin());
  }

  reverse_iterator rend() noexcept {
    return reverse_iterator(base.rend());
  }

  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(base.rend());
  }

  const_iterator cbegin() const noexcept {
    return const_iterator(base.cbegin());
  }

  const_iterator cend() const noexcept {
    return const_iterator(base.cend());
  }

  const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(base.crbegin());
  }

  const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(base.crend());
  }

  reference front() noexcept {
    return *begin();
  }

  const_reference front() const noexcept {
    return *begin();
  }

  reference back() noexcept {
    return *(end() - 1);
  }

  const_reference back() const noexcept {
    return *(end() - 1);
  }

  pointer data() noexcept = delete;

  const_pointer data() const noexcept = delete;

  reference at(size_type pos) {
    return *(base.at(pos));
  }

  const_reference at(size_type pos) const {
    return *(base.at(pos));
  }

  reference operator[](size_type pos) noexcept {
    return *(base[pos]);
  }

  const_reference operator[](size_type pos) const noexcept {
    return *(base[pos]);
  }

  bool empty() const noexcept {
    return base.empty();
  }

  size_type size() const noexcept {
    return base.size();
  }

  size_type max_size() const noexcept {
    return base.max_size();
  }

  size_type capacity() const noexcept {
    return base.capacity();
  }

  void resize(size_type n) {
    base.resize(n);
  }

  void resize(size_type n, value_type &value) = delete;

  void reserve(size_type n) {
    base.reserve(n);
  }

  void shrink_to_fit() {
    base.shrink_to_fit();
  }

  void push_back(value_type &value) {
    base.push_back(&value);
  }

  void push_back(value_type &&value) = delete;

  template <typename... Args>
  void emplace_back(Args &&... args) = delete;

  void pop_back() noexcept {
    base.pop_back();
  }

  iterator insert(const_iterator pos, value_type &value) {
    return base.insert(pos, &value);
  }

  iterator insert(const_iterator pos, value_type &&value) = delete;

  template <typename Iter>
  iterator insert(const_iterator pos, Iter first, Iter last) = delete;

  iterator insert(const_iterator pos, std::initializer_list<value_type> init) = delete;

  template <typename... Args>
  iterator emplace(const_iterator pos, Args &&... args) = delete;

  iterator erase(const_iterator pos) {
    return iterator(base.erase(pos));
  }

  iterator erase(const_iterator first, const_iterator last) {
    return iterator(first.base(), last.base());
  }

  void swap(ref_vector &other) noexcept {
    base.swap(other.base);
  }

  void clear() noexcept {
    base.clear();
  }

 private:
  base_vector base;
};

template <typename T, typename Alloc>
inline bool operator==(const ref_vector<T, Alloc> &lhs, const ref_vector<T, Alloc> &rhs) {
  return (lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}

template <typename T, typename Alloc>
inline bool operator!=(const ref_vector<T, Alloc> &lhs, const ref_vector<T, Alloc> &rhs) {
  return !(lhs == rhs);
}

template <typename T, typename Alloc>
inline bool operator<(const ref_vector<T, Alloc> &lhs, const ref_vector<T, Alloc> &rhs) {
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T, typename Alloc>
inline bool operator>(const ref_vector<T, Alloc> &lhs, const ref_vector<T, Alloc> &rhs) {
  return rhs < lhs;
}

template <typename T, typename Alloc>
inline bool operator<=(const ref_vector<T, Alloc> &lhs, const ref_vector<T, Alloc> &rhs) {
  return !(rhs < lhs);
}

template <typename T, typename Alloc>
inline bool operator>=(const ref_vector<T, Alloc> &lhs, const ref_vector<T, Alloc> &rhs) {
  return !(lhs < rhs);
}
}}

namespace std {
template <typename T, typename Alloc>
inline void swap(maple::utils::ref_vector<T, Alloc> &lhs, maple::utils::ref_vector<T, Alloc> &rhs) noexcept {
  lhs.swap(rhs);
}
}
#endif
