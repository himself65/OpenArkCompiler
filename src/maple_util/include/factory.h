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
#ifndef MAPLE_UTIL_INCLUDE_FACTORY_H
#define MAPLE_UTIL_INCLUDE_FACTORY_H
#include <map>
#include <memory>
#include <functional>

namespace maple {
template <typename TKey, typename TObject, typename... TArgs>
class ObjectFactory final {
 public:
  using key_type = TKey;
  using creator_type = std::function<std::unique_ptr<TObject>(TArgs...)>;

  void Register(const key_type &key, creator_type func) {
    if (creator.find(key) == creator.end()) {
      creator[key] = func;
    }
  }

  std::unique_ptr<TObject> Create(const key_type &key, TArgs ...args) const {
    auto it = creator.find(key);
    return it == creator.end() ? std::unique_ptr<TObject>() : (it->second)(std::forward<TArgs>(args)...);
  }

  template <typename TObjectImpl>
  static std::unique_ptr<TObject> DefaultCreator(TArgs ...args) {
    return std::make_unique<TObjectImpl>(std::forward<TArgs>(args)...);
  }

  static ObjectFactory &ins() {
    static ObjectFactory factory;
    return factory;
  }

  ObjectFactory(const ObjectFactory&) = delete;
  ObjectFactory &operator=(const ObjectFactory&) = delete;
  ObjectFactory(const ObjectFactory&&) = delete;
  ObjectFactory &operator=(const ObjectFactory&&) = delete;

 private:
  ObjectFactory() = default;
  ~ObjectFactory() = default;

 private:
  using CreatorHolder = std::map<key_type, creator_type>;
  CreatorHolder creator;
};

template <typename TFactory, typename TFactory::key_type Key, typename TObjectImpl>
inline void RegisterFactoryObject() {
  TFactory::ins().Register(Key, TFactory::template DefaultCreator<TObjectImpl>);
}

template <typename TFactory, typename... TArgs>
inline auto CreateProductObject(const typename TFactory::key_type &key, TArgs &&...args) {
  return TFactory::ins().Create(key, std::forward<TArgs>(args)...);
}

template <typename TKey, typename TRet, typename... TArgs>
class FunctionFactory final {
 public:
  using key_type = TKey;
  using creator_type = std::function<TRet(TArgs...)>;

  void Register(const key_type &key, creator_type func) {
    if (creator.find(key) == creator.end()) {
      creator[key] = func;
    }
  }

  creator_type Create(const key_type &key) const {
    auto it = creator.find(key);
    return it == creator.end() ? nullptr : it->second;
  }

  static FunctionFactory &ins() {
    static FunctionFactory factory;
    return factory;
  }

  FunctionFactory(const FunctionFactory&) = delete;
  FunctionFactory &operator=(const FunctionFactory&) = delete;
  FunctionFactory(const FunctionFactory&&) = delete;
  FunctionFactory &operator=(const FunctionFactory&&) = delete;

 private:
  FunctionFactory() = default;
  ~FunctionFactory() = default;

 private:
  using CreatorHolder = std::map<key_type, creator_type>;
  CreatorHolder creator;
};

template <typename TFactory>
inline void RegisterFactoryFunction(const typename TFactory::key_type &key, typename TFactory::creator_type func) {
  TFactory::ins().Register(key, func);
}

template <typename TFactory>
inline auto CreateProductFunction(const typename TFactory::key_type &key) {
  return TFactory::ins().Create(key);
}
}  // namespace maple

#endif  // MAPLE_UTIL_INCLUDE_FACTORY_H
