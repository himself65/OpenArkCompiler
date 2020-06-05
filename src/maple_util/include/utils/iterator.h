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
#ifndef MAPLE_UTIL_INCLUDE_ITERATOR_H
#define MAPLE_UTIL_INCLUDE_ITERATOR_H
#include <iterator>
#include "meta.h"

namespace maple {
namespace utils {
template <typename Iterator, typename Container>
struct mpl_iterator_traits {
  using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
  using value_type = typename std::iterator_traits<Iterator>::value_type;
  using difference_type = typename std::iterator_traits<Iterator>::difference_type;
  using pointer = typename std::iterator_traits<Iterator>::pointer;
  using reference = typename std::iterator_traits<Iterator>::reference;

  static reference operator_dereference(Iterator iter) {
    return *iter;
  };

  static Iterator operator_arrow(Iterator iter) {
    return iter;
  }

  static reference operator_bracket(Iterator iter, difference_type n) {
    return iter[n];
  }
};

template <typename Iterator, typename Container>
class mpl_iterator {
  using Traits = mpl_iterator_traits<Iterator, Container>;
 public:
  using iterator_category = typename Traits::iterator_category;
  using value_type = typename Traits::value_type;
  using difference_type = typename Traits::difference_type;
  using pointer = typename Traits::pointer;
  using reference = typename Traits::reference;

  explicit mpl_iterator(Iterator iter)
      : iter(iter) {}

  template <typename U, typename = std::enable_if_t<ptr::const_of_v<U, pointer>>>
  mpl_iterator(const mpl_iterator<U, Container> &iter)
      : iter(iter.base()) {}

  ~mpl_iterator() noexcept = default;

  reference operator*() const noexcept {
    return Traits::operator_dereference(iter);
  }

  pointer operator->() const noexcept {
    return Traits::operator_arrow(iter);
  }

  mpl_iterator &operator++() noexcept {
    ++iter;
    return *this;
  }

  mpl_iterator operator++(int) noexcept {
    return mpl_iterator(iter++);
  }

  mpl_iterator &operator--() noexcept {
    --iter;
    return *this;
  }

  mpl_iterator operator--(int) noexcept {
    return mpl_iterator(iter--);
  }

  reference operator[](difference_type n) const noexcept {
    return Traits::operator_bracket(iter, n);
  }

  mpl_iterator &operator+=(difference_type n) noexcept {
    iter += n;
    return *this;
  }

  mpl_iterator &operator-=(difference_type n) noexcept {
    iter -= n;
    return *this;
  }

  Iterator base() const noexcept {
    return iter;
  }

 private:
  Iterator iter;
};

template <typename T, typename Container>
inline bool operator==(const mpl_iterator<T, Container> &lhs, const mpl_iterator<T, Container> &rhs) noexcept {
  return lhs.base() == rhs.base();
}

template <typename T, typename U, typename Container, typename = std::enable_if_t<ptr::is_ncv_same_v<T, U>>>
inline bool operator==(const mpl_iterator<T, Container> &lhs, const mpl_iterator<U, Container> &rhs) noexcept {
  return lhs.base() == rhs.base();
}

template <typename T, typename Container>
inline bool operator!=(const mpl_iterator<T, Container> &lhs, const mpl_iterator<T, Container> &rhs) noexcept {
  return !(lhs == rhs);
}

template <typename T, typename U, typename Container, typename = std::enable_if_t<ptr::is_ncv_same_v<T, U>>>
inline bool operator!=(const mpl_iterator<T, Container> &lhs, const mpl_iterator<U, Container> &rhs) noexcept {
  return !(lhs == rhs);
}

template <typename T, typename Container>
inline bool operator<(const mpl_iterator<T, Container> &lhs, const mpl_iterator<T, Container> &rhs) noexcept {
  return lhs.base() < rhs.base();
}

template <typename T, typename U, typename Container, typename = std::enable_if_t<ptr::is_ncv_same_v<T, U>>>
inline bool operator<(const mpl_iterator<T, Container> &lhs, const mpl_iterator<U, Container> &rhs) noexcept {
  return lhs.base() < rhs.base();
}

template <typename T, typename Container>
inline bool operator<=(const mpl_iterator<T, Container> &lhs, const mpl_iterator<T, Container> &rhs) noexcept {
  return lhs.base() <= rhs.base();
}

template <typename T, typename U, typename Container, typename = std::enable_if_t<ptr::is_ncv_same_v<T, U>>>
inline bool operator<=(const mpl_iterator<T, Container> &lhs, const mpl_iterator<U, Container> &rhs) noexcept {
  return lhs.base() <= rhs.base();
}

template <typename T, typename Container>
inline bool operator>(const mpl_iterator<T, Container> &lhs, const mpl_iterator<T, Container> &rhs) noexcept {
  return lhs.base() > rhs.base();
}

template <typename T, typename U, typename Container, typename = std::enable_if_t<ptr::is_ncv_same_v<T, U>>>
inline bool operator>(const mpl_iterator<T, Container> &lhs, const mpl_iterator<U, Container> &rhs) noexcept {
  return lhs.base() > rhs.base();
}

template <typename T, typename Container>
inline bool operator>=(const mpl_iterator<T, Container> &lhs, const mpl_iterator<T, Container> &rhs) noexcept {
  return lhs.base() >= rhs.base();
}

template <typename T, typename U, typename Container, typename = std::enable_if_t<ptr::is_ncv_same_v<T, U>>>
inline bool operator>=(const mpl_iterator<T, Container> &lhs, const mpl_iterator<U, Container> &rhs) noexcept {
  return lhs.base() >= rhs.base();
}

template <typename T, typename Container>
mpl_iterator<T, Container> operator+(const mpl_iterator<T, Container> &iter,
                                     typename mpl_iterator<T, Container>::difference_type n) noexcept {
  return mpl_iterator<T, Container>(iter.base() + n);
}

template <typename T, typename Container>
mpl_iterator<T, Container> operator+(typename mpl_iterator<T, Container>::difference_type n,
                                     const mpl_iterator<T, Container> &iter) noexcept {
  return mpl_iterator<T, Container>(n + iter.base());
}

template <typename T, typename Container>
mpl_iterator<T, Container> operator-(const mpl_iterator<T, Container> &iter,
                                     typename mpl_iterator<T, Container>::difference_type n) noexcept {
  return mpl_iterator<T, Container>(iter.base() - n);
}

template <typename T, typename Container>
inline auto operator-(const mpl_iterator<T, Container> &lhs, const mpl_iterator<T, Container> &rhs) noexcept
-> typename mpl_iterator<T, Container>::difference_type {
  return lhs.base() - rhs.base();
}

template <typename T, typename U, typename Container, typename = std::enable_if_t<ptr::is_ncv_same_v<T, U>>>
inline auto operator-(const mpl_iterator<T, Container> &lhs, const mpl_iterator<U, Container> &rhs) noexcept
-> decltype(lhs.base() - rhs.base()) {
  return lhs.base() - rhs.base();
}
}}

#endif //MAPLE_UTIL_INCLUDE_ITERATOR_H
