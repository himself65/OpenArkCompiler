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
#include "fe_utils_java.h"
#include "name_mangler.h"

namespace maple {
std::vector<std::string> FEUtilJava::SolveMethodSignature(std::string signature, bool inMpl) {
  if (inMpl) {
    signature = NameMangler::DecodeName(signature);
  }
  std::vector<std::string> ans;
  size_t pos1 = signature.find('(');
  size_t pos2 = signature.find(')');
  if (pos1 == std::string::npos || pos2 == std::string::npos || pos1 > pos2) {
    CHECK_FATAL(false, "invalid method signature %s", signature.c_str());
  }
  std::string paramTypeNames = signature.substr(pos1 + 1, pos2 - pos1 - 1);
  std::string retTypeName = signature.substr(pos2 + 1);
  ans.push_back(inMpl ? NameMangler::EncodeName(retTypeName) : retTypeName);
  while (paramTypeNames.length() > 0) {
    std::string typeName = SolveParamNameInJavaFormat(paramTypeNames);
    ans.push_back(inMpl ? NameMangler::EncodeName(typeName) : typeName);
    paramTypeNames = paramTypeNames.substr(typeName.length());
  }
  return ans;
}

std::string FEUtilJava::SolveParamNameInJavaFormat(const std::string &signature) {
  if (signature.empty()) {
    return "";
  }
  char c = signature[0];
  switch (c) {
    case '[':
      return "[" + SolveParamNameInJavaFormat(signature.substr(1));
    case 'L': {
      size_t pos = signature.find(';');
      CHECK_FATAL(pos != std::string::npos, "invalid type %s", signature.c_str());
      return signature.substr(0, pos + 1);
    }
    default: {
      std::string ans = "";
      ans.push_back(c);
      return ans;
    }
  }
}
}  // namespace maple