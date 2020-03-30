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
#ifndef MAPLE_UTIL_INCLUDE_PTR_LIST_REF_H
#define MAPLE_UTIL_INCLUDE_PTR_LIST_REF_H
#include <iterator>

#include "mpl_logging.h"

namespace maple {
template <typename T>
class PtrListNodeBase {
 public:
  PtrListNodeBase() = default;
  ~PtrListNodeBase() = default;
  T *GetPrev() const {
    return prev;
  }

  T *GetNext() const {
    return next;
  }

  void SetPrev(T *ptr) {
    prev = ptr;
  }

  void SetNext(T *ptr) {
    next = ptr;
  }

 private:
  T *prev = nullptr;
  T *next = nullptr;
};

// wrap iterator to run it backwards
template <typename T>
class ReversePtrListRefIterator {
 public:
  using iterator_category = typename std::iterator_traits<T>::iterator_category;
  using value_type = typename std::iterator_traits<T>::value_type;
  using difference_type = typename std::iterator_traits<T>::difference_type;
  using pointer = typename std::iterator_traits<T>::pointer;
  using reference = typename std::iterator_traits<T>::reference;

  using iterator_type = T;

  ReversePtrListRefIterator() : current() {}

  explicit ReversePtrListRefIterator(T right) : current(right) {}

  template <class Other>
  ReversePtrListRefIterator(const ReversePtrListRefIterator<Other> &right) : current(right.base()) {}

  template <class Other>
  ReversePtrListRefIterator &operator=(const ReversePtrListRefIterator<Other> &right) {
    current = right.base();
    return (*this);
  }

  ~ReversePtrListRefIterator() = default;

  T base() const {
    return current;
  }

  reference operator*() const {
    return *current;
  }

  pointer operator->() const {
    return &(operator*());
  }

  ReversePtrListRefIterator &operator++() {
    --current;
    return (*this);
  }

  ReversePtrListRefIterator operator++(int) {
    ReversePtrListRefIterator tmp = *this;
    --current;
    return (tmp);
  }

  ReversePtrListRefIterator &operator--() {
    ++current;
    return (*this);
  }

  ReversePtrListRefIterator operator--(int) {
    ReversePtrListRefIterator tmp = *this;
    ++current;
    return (tmp);
  }

  bool operator==(const ReversePtrListRefIterator &Iterator) const {
    return this->base() == Iterator.base();
  }

  bool operator!=(const ReversePtrListRefIterator &Iterator) const {
    return !(*this == Iterator);
  }

 protected:
  T current;
};

template <typename T>
class PtrListRefIterator {
 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = T;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using reference = T&;
  using const_pointer = const T*;
  using const_reference = const T&;

  PtrListRefIterator() = default;

  explicit PtrListRefIterator(pointer _Ptr) : ptr(_Ptr) {}

  template <typename U, typename = std::enable_if_t<std::is_same<U, std::remove_const_t<T>>::value>>
  PtrListRefIterator(const PtrListRefIterator<U> &_Iter) : ptr(_Iter.d()) {}

  ~PtrListRefIterator() = default;

  pointer d() const {
    return ptr;
  }

  reference operator*() const {
    return *ptr;
  }

  pointer operator->() const {
    return ptr;
  }

  PtrListRefIterator &operator++() {
    this->ptr = this->ptr->GetNext();
    return *this;
  }

  PtrListRefIterator &operator--() {
    this->ptr = this->ptr->GetPrev();
    return *this;
  }

  PtrListRefIterator operator++(int) {
    PtrListRefIterator it = *this;
    ++(*this);
    return it;
  }

  PtrListRefIterator operator--(int) {
    PtrListRefIterator it = *this;
    --(*this);
    return it;
  }

  bool operator==(const PtrListRefIterator &Iterator) const {
    return this->ptr == Iterator.ptr;
  }

  bool operator!=(const PtrListRefIterator &Iterator) const {
    return !(*this == Iterator);
  }

 private:
  pointer ptr = nullptr;
};

template <typename T>
class PtrListRef {
 public:
  using value_type = T;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;

  using iterator = PtrListRefIterator<T>;
  using const_iterator = PtrListRefIterator<const T>;
  using reverse_iterator = ReversePtrListRefIterator<iterator>;
  using const_reverse_iterator = ReversePtrListRefIterator<const_iterator>;

  PtrListRef() = default;
  explicit PtrListRef(pointer _Value) : first(_Value), last(_Value) {}

  PtrListRef(pointer _First, pointer _Last) : first(_First), last(_Last == nullptr ? _First : _Last) {}

  ~PtrListRef() = default;

  iterator begin() {
    return iterator(this->first);
  }

  const_iterator begin() const {
    return const_iterator(this->first);
  }

  const_iterator cbegin() const {
    return const_iterator(this->first);
  }

  iterator end() {
    return iterator(this->last == nullptr ? nullptr : this->last->GetNext());
  }

  const_iterator end() const {
    return const_iterator(this->last == nullptr ? nullptr : this->last->GetNext());
  }

  const_iterator cend() const {
    return const_iterator(this->last == nullptr ? nullptr : this->last->GetNext());
  }

  reverse_iterator rbegin() {
    return reverse_iterator(iterator(this->last));
  }

  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(const_iterator(this->last));
  }

  const_reverse_iterator crbegin() const {
    return const_reverse_iterator(const_iterator(this->last));
  }

  reverse_iterator rend() {
    return reverse_iterator(iterator(this->first == nullptr ? nullptr : this->first->GetPrev()));
  }

  const_reverse_iterator rend() const {
    return const_reverse_iterator(const_iterator(this->first == nullptr ? nullptr : this->first->GetPrev()));
  }

  const_reverse_iterator crend() const {
    return const_reverse_iterator(const_iterator(this->first == nullptr ? nullptr : this->first->GetPrev()));
  }

  reference front() {
    return *(this->first);
  }

  reference back() {
    return *(this->last);
  }

  const_reference front() const {
    return *(this->first);
  }

  const_reference back() const {
    return *(this->last);
  }

  bool empty() const {
    return first == nullptr;
  }

  void update_front(pointer _Value) {
    if (_Value != nullptr) {
      _Value->SetPrev(nullptr);
    }
    this->first = _Value;
  }

  void push_front(pointer _Value) {
    if (this->last == nullptr) {
      this->first = _Value;
      this->last = _Value;
      _Value->SetPrev(nullptr);
      _Value->SetNext(nullptr);
    } else {
      ASSERT(this->first != nullptr, "null ptr check");
      this->first->SetPrev(_Value);
      _Value->SetPrev(nullptr);
      _Value->SetNext(this->first);
      this->first = _Value;
    }
  }

  void pop_front() {
    if (this->first == nullptr) {
      return;
    }

    this->first = this->first->GetNext();
    if (this->first != nullptr) {
      this->first->SetPrev(nullptr);
    }
  }

  void update_back(pointer _Value) {
    if (_Value != nullptr) {
      _Value->SetNext(nullptr);
    }
    this->last = _Value;
  }

  void push_back(pointer _Value) {
    if (this->last == nullptr) {
      this->first = _Value;
      this->last = _Value;
      _Value->SetPrev(nullptr);
    } else {
      this->last->SetNext(_Value);
      _Value->SetPrev(this->last);
      this->last = _Value;
    }
    _Value->SetNext(nullptr);
  }

  void pop_back() {
    if (this->last == nullptr) {
      return;
    }

    if (this->last->GetPrev() == nullptr) {
      this->first = nullptr;
      this->last = nullptr;
    } else {
      this->last = this->last->GetPrev();
      this->last->SetNext(nullptr);
    }
  }

  void insert(const_iterator _Where, pointer _Value) {
    if (_Where == const_iterator(this->first)) {
      this->push_front(_Value);
    } else if (_Where == this->cend()) {
      this->push_back(_Value);
    } else {
      // `_Where` stands for the position, however we made the data and node combined, so a const_cast is needed.
      auto *ptr = const_cast<T*>(&*_Where);
      _Value->SetPrev(ptr->GetPrev());
      _Value->SetNext(ptr);
      _Value->GetPrev()->SetNext(_Value);
      ptr->SetPrev(_Value);
    }
  }

  void insert(const_pointer _Where, pointer _Value) {
    this->insert(const_iterator(_Where), _Value);
  }

  void insertAfter(const_iterator _Where, pointer _Value) {
    if (_Where == const_iterator(nullptr)) {
      this->push_front(_Value);
    } else if (_Where == const_iterator(this->last)) {
      this->push_back(_Value);
    } else {
      // `_Where` stands for the position, however we made the data and node combined, so a const_cast is needed.
      auto *ptr = const_cast<T*>(&*_Where);
      _Value->SetPrev(ptr);
      _Value->SetNext(ptr->GetNext());
      _Value->GetNext()->SetPrev(_Value);
      ptr->SetNext(_Value);
    }
  }

  void insertAfter(const_pointer _Where, pointer _Value) {
    this->insertAfter(const_iterator(_Where), _Value);
  }

  void splice(const_iterator _Where, PtrListRef &_Other) {
    ASSERT(!_Other.empty(), "NYI");
    if (this->empty()) {
      this->first = &(_Other.front());
      this->last = &(_Other.back());
    } else if (_Where == this->cend() || _Where == const_iterator(this->last)) {
      ASSERT(this->last != nullptr, "null ptr check");
      this->last->SetNext(&(_Other.front()));
      _Other.front().SetPrev(this->last);
      this->last = &(_Other.back());
    } else {
      ASSERT(to_ptr(_Where) != nullptr, "null ptr check");
      ASSERT(_Where->GetNext() != nullptr, "null ptr check");
      // `_Where` stands for the position, however we made the data and node combined, so a const_cast is needed.
      auto *ptr = const_cast<T*>(&*_Where);
      _Other.front().SetPrev(ptr);
      _Other.back().SetNext(ptr->GetNext());
      ptr->GetNext()->SetPrev(&(_Other.back()));
      ptr->SetNext(&(_Other.front()));
    }
  }

  void splice(const_pointer _Where, PtrListRef &_Other) {
    splice(const_iterator(_Where), _Other);
  }

  void clear() {
    this->first = nullptr;
    this->last = nullptr;
  }

  iterator erase(const_iterator _Where) {
    if (_Where == this->cbegin() && _Where == this->rbegin().base()) {
      this->first = nullptr;
      this->last = nullptr;
    } else if (_Where == this->cbegin()) {
      // `_Where` stands for the position, however we made the data and node combined, so a const_cast is needed.
      auto *ptr = const_cast<T*>(&*_Where);
      this->first = ptr->GetNext();
      ASSERT(this->first != nullptr, "null ptr check");
      this->first->SetPrev(nullptr);
    } else if (_Where == this->rbegin().base()) {
      pop_back();
    } else {
      ASSERT(_Where->GetPrev() != nullptr, "null ptr check");
      // `_Where` stands for the position, however we made the data and node combined, so a const_cast is needed.
      auto *ptr = const_cast<T*>(&*_Where);
      ptr->GetPrev()->SetNext(ptr->GetNext());
      ptr->GetNext()->SetPrev(ptr->GetPrev());
    }
    return iterator(nullptr);
  }

  iterator erase(const_pointer _Where) {
    return this->erase(const_iterator(_Where));
  }

 private:
  T *first = nullptr;
  T *last = nullptr;
};

template <typename Iterator>
auto to_ptr(Iterator it) -> typename std::iterator_traits<Iterator>::pointer {
  return it.d();
}

template <typename Iterator>
auto to_ptr(ReversePtrListRefIterator<Iterator> it) -> typename std::iterator_traits<Iterator>::pointer {
  return it.base().d();
}
}  // namespace maple
#endif  // MAPLE_UTIL_INCLUDE_PTR_LIST_REF_H
