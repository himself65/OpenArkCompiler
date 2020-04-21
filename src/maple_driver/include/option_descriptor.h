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
#ifndef MAPLE_OPTION_DESCRIPTOR_H
#define MAPLE_OPTION_DESCRIPTOR_H
#include <vector>
#include <string>
#include <algorithm>

namespace mapleOption {
enum BuildType {
  kBuildTypeAll,
  kBuildTypeProduct,
  kBuildTypeDebug,
  kBuildTypeRelease,
  kBuildTypeExperimental
};

enum ArgCheckPolicy {
  kArgCheckPolicyUnknown,
  kArgCheckPolicyNone,
  kArgCheckPolicyOptional,
  kArgCheckPolicyRequired,
  kArgCheckPolicyNumeric,
  kArgCheckPolicyBool
};

constexpr unsigned int kMaxExtraOptions = 10;

struct Descriptor {
  // Unique option index
  const unsigned int index;

  // Unique option type
  const int type;

  // Short form option key
  const char * const shortOption;

  // Long form option key
  const char * const longOption;

  // The option can be seen in which build type.
  const BuildType enableBuildType;

  // Should we have a parameter?
  const ArgCheckPolicy checkPolicy;

  // Help info
  const char * const help;

  const char * const exeName;

  // option key mapping to target tool
  const std::vector<std::string> extras;

  bool IsEnabledForCurrentBuild() const {
    switch (enableBuildType) {
      case BuildType::kBuildTypeAll:
        return true;
      case BuildType::kBuildTypeDebug:
        return true;
      case BuildType::kBuildTypeRelease:
        return true;
      default:
        // should never reach
        return true;
      }
    }
};

class Option {
 public:
  Option(Descriptor desc, const std::string &optionKey, const std::string &args)
      : descriptor(desc), optionKey(optionKey), args(args) {}

  ~Option() = default;

  unsigned int Index() const {
    return descriptor.index;
  }

  unsigned int Type() const {
    return descriptor.type;
  }

  const std::string &Args() const {
    return args;
  }

  const std::string &OptionKey() const {
    return optionKey;
  }

  bool HasExtra() const {
    return (descriptor.extras.size() > 0);
  }

  std::vector<std::string> GetExtras() const {
    auto ret = std::vector<std::string>();
    unsigned int index = 0;
    while (index < kMaxExtraOptions && index < descriptor.extras.size()) {
      ret.push_back(descriptor.extras[index++]);
    }
    return ret;
  }

  bool FindExtra(const std::string &exeName) const {
    auto item = std::find(descriptor.extras.begin(), descriptor.extras.end(), exeName);
    if (item == descriptor.extras.end()) {
      return false;
    }
    return true;
  }

 private:
  Descriptor descriptor;
  const std::string optionKey;
  const std::string args;
};

enum EnabledIndex {
  kDisable,
  kEnable
};
}  // namespace mapleOption

#endif //MAPLE_OPTION_DESCRIPTOR_H
