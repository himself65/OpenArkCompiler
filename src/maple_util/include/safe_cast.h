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
#ifndef MAPLE_UTIL_INCLUDE_SAFE_CAST_H
#define MAPLE_UTIL_INCLUDE_SAFE_CAST_H
#include "utils/meta.h"

namespace maple {

template <typename ToT>
struct safe_cast_condition : std::false_type {};

#define REGISTER_SAFE_CAST(type, condition)                             \
template <>                                                             \
struct safe_cast_condition<type> : std::true_type {                     \
  template <typename FromT>                                             \
  static inline bool DoIt(const FromT &from) {                          \
    return (condition);                                                 \
  }                                                                     \
}

namespace __impl {
template <typename ToT, typename FromT,
  typename = std::enable_if_t<std::is_base_of<FromT, ToT>::value>>
struct instance_of_impl {
  static inline bool DoIt(const FromT &from) {
    return (safe_cast_condition<ToT>::DoIt(from));
  }
};

template <typename ToT, typename FromT>
struct instance_of_impl<ToT, FromT, typename std::enable_if_t<std::is_base_of<ToT, FromT>::value>> {
  static inline bool DoIt(const FromT&) {
    return true;
  }
};

template <typename ToT, typename FromT>
struct enabled_safe_cast
    : utils::meta_or<std::is_base_of<ToT, FromT>, safe_cast_condition<ToT>>::type {};
}

template <typename ToT, typename FromT,
  typename = std::enable_if_t<__impl::enabled_safe_cast<ToT, FromT>::value>>
inline bool instance_of(FromT &from) {
  return __impl::instance_of_impl<ToT, FromT>::DoIt(from);
}

template <typename ToT, typename FromT,
  typename = std::enable_if_t<__impl::enabled_safe_cast<ToT, FromT>::value>>
inline bool instance_of(FromT *from) {
  return (from != nullptr && instance_of<ToT>(*from));
}

template <typename ToT, typename FromT,
  typename RetT = std::conditional_t<
    std::is_const<FromT>::value || std::is_const<std::remove_pointer_t<ToT>>::value,
    std::add_pointer_t<std::add_const_t<std::remove_cv_t<ToT>>>,
    std::add_pointer_t<std::remove_cv_t<ToT>>>,
  typename = std::enable_if_t<__impl::enabled_safe_cast<ToT, FromT>::value>>
inline RetT safe_cast(FromT &from) {
  return (instance_of<ToT>(from) ? static_cast<RetT>(&from) : nullptr);
}

template <typename ToT, typename FromT,
  typename RetT = std::conditional_t<
    std::is_const<FromT>::value || std::is_const<std::remove_pointer_t<ToT>>::value,
    std::add_pointer_t<std::add_const_t<std::remove_cv_t<ToT>>>,
    std::add_pointer_t<std::remove_cv_t<ToT>>>,
  typename = std::enable_if_t<__impl::enabled_safe_cast<ToT, FromT>::value>>
inline RetT safe_cast(FromT *from) {
  return (instance_of<ToT>(from) ? static_cast<RetT>(from) : nullptr);
}

}
#endif //MAPLE_UTIL_INCLUDE_SAFE_CAST_H
