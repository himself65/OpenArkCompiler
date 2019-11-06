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
#ifndef MAPLE_UTIL_INCLUDE_SAFE_CAST_H
#define MAPLE_UTIL_INCLUDE_SAFE_CAST_H

namespace maple {

template<typename T>
struct ExtractCode {
};

template<typename DeriveT, typename T,
  typename = std::enable_if_t<std::is_base_of<T, DeriveT>::value>>
inline bool instance_of(T &base) {
  return safe_cast_traits(base) == ExtractCode<DeriveT>::value;
}

template<typename DeriveT, typename T,
  typename = std::enable_if_t<std::is_base_of<T, DeriveT>::value>>
inline bool instance_of(T *base) {
  return (base != nullptr && instance_of<DeriveT>(*base));
}

template<typename DeriveT, typename T,
  typename RetT = std::conditional_t<
    std::is_const<T>::value || std::is_const<std::remove_pointer_t<DeriveT>>::value,
    std::add_pointer_t<std::add_const_t<std::remove_cv_t<DeriveT>>>,
    std::add_pointer_t<std::remove_cv_t<DeriveT>>>,
  typename = std::enable_if_t<std::is_base_of<T, DeriveT>::value>>
inline RetT safe_cast(T &base) {
  return (safe_cast_traits(base) == ExtractCode<DeriveT>::value ? static_cast<RetT>(&base) : nullptr);
}

template<typename DeriveT, typename T,
  typename RetT = std::conditional_t<
    std::is_const<T>::value || std::is_const<std::remove_pointer_t<DeriveT>>::value,
    std::add_pointer_t<std::add_const_t<std::remove_cv_t<DeriveT>>>,
    std::add_pointer_t<std::remove_cv_t<DeriveT>>>,
  typename = std::enable_if_t<std::is_base_of<T, DeriveT>::value>>
inline RetT safe_cast(T *base) {
  return ((base != nullptr && safe_cast_traits(*base) == ExtractCode<DeriveT>::value) ?
          static_cast<RetT>(base) : nullptr);
}

}

#endif //MAPLE_UTIL_INCLUDE_SAFE_CAST_H
