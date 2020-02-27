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
#include "class_hierarchy.h"
#include <iostream>
#include <fstream>
#include "option.h"

// Class Hierarchy Analysis
// This phase is a foundation phase of compilation. This phase build
// the class hierarchy for both this module and all modules it depends
// on. So many phases rely on this phase's analysis result, such as
// call graph, ssa and so on.
// The main procedure shows as following.
// A. Based on the information read from mplts, it collects all class that
//    declared in modules. And creates a Klass for each class.
// B. Fill class method info. Connect superclass<->subclass and
//    interface->implementation edges.
// C. Tag All Throwable class and its child class.
// D. In the case of "class C implements B; interface B extends A;",
//    we need to add a link between C and A. So we recursively traverse
//    Klass and collect all interfaces it implements.
// E. Topological Sort
// F. Based on Topological Sort Order, for each virtual method in a class,
//    we collect all its potential implementation. If the number of
//    potential implementations is 1, it means all virtual calls to this
//    method can be easily devirtualized.
namespace maple {
bool KlassHierarchy::traceFlag = false;

bool IsSystemPreloadedClass(const std::string &className) {
  return false;
}

Klass::Klass(MIRStructType *type, MapleAllocator *alc)
    : structType(type),
      alloc(alc),
      superKlasses(alloc->Adapter()),
      subKlasses(alloc->Adapter()),
      implKlasses(alloc->Adapter()),
      implInterfaces(alloc->Adapter()),
      methods(alloc->Adapter()),
      strIdx2Method(alloc->Adapter()),
      strIdx2CandidateMap(alloc->Adapter()) {
  ASSERT(type != nullptr, "type is nullptr in Klass::Klass!");
  ASSERT(type->GetKind() == kTypeClass || type->GetKind() == kTypeInterface ||
         type->IsIncomplete(), "runtime check error");
}

void Klass::DumpKlassMethods() const {
  if (methods.empty()) {
    return;
  }
  LogInfo::MapleLogger() << "   class member methods:\n";
  for (MIRFunction *method : methods) {
    LogInfo::MapleLogger() << "   \t" << method->GetName() << " , ";
    method->GetFuncSymbol()->GetAttrs().DumpAttributes();
    LogInfo::MapleLogger() << "\n";
  }
  for (const auto &m2cPair : strIdx2CandidateMap) {
    LogInfo::MapleLogger() << "   \t" << GlobalTables::GetStrTable().GetStringFromStrIdx(m2cPair.first)
                           << "   , # of target:" << m2cPair.second->size() << "\n";
  }
}

void Klass::DumpKlassImplKlasses() const {
  if (implKlasses.empty()) {
    return;
  }
  LogInfo::MapleLogger() << "  implemented by:\n";
  for (Klass *implKlass : implKlasses) {
    LogInfo::MapleLogger() << "   \t@implbyclass_idx " << implKlass->structType->GetTypeIndex() << "\n";
  }
}

void Klass::DumpKlassImplInterfaces() const {
  if (implInterfaces.empty()) {
    return;
  }
  LogInfo::MapleLogger() << "  implements:\n";
  for (Klass *interface : implInterfaces) {
    LogInfo::MapleLogger() << "   \t@implinterface_idx " << interface->structType->GetTypeIndex() << "\n";
  }
}

void Klass::DumpKlassSuperKlasses() const {
  if (superKlasses.empty()) {
    return;
  }
  LogInfo::MapleLogger() << "   superclasses:\n";
  for (Klass *superKlass : superKlasses) {
    LogInfo::MapleLogger() << "   \t@superclass_idx " << superKlass->structType->GetTypeIndex() << "\n";
  }
}

void Klass::DumpKlassSubKlasses() const {
  if (subKlasses.empty()) {
    return;
  }
  LogInfo::MapleLogger() << "   subclasses:\n";
  for (Klass *subKlass : subKlasses) {
    LogInfo::MapleLogger() << "   \t@subclass_idx " << subKlass->structType->GetTypeIndex() << "\n";
  }
}

void Klass::Dump() const {
  // Dump detailed class info
  LogInfo::MapleLogger() << "class \" " << GetKlassName() << " \" @class_id " << structType->GetTypeIndex()
                         << "\n";
  DumpKlassSuperKlasses();
  DumpKlassSubKlasses();
  DumpKlassImplInterfaces();
  DumpKlassImplKlasses();
  DumpKlassMethods();
}

MIRFunction *Klass::GetClosestMethod(GStrIdx funcName) const {
  MapleVector<MIRFunction*> *cands = GetCandidates(funcName);
  if (cands != nullptr && !cands->empty()) {
    return cands->at(0);
  }
  return nullptr;
}

void Klass::DelMethod(const MIRFunction &func) {
  if (GetMethod(func.GetBaseFuncNameWithTypeStrIdx()) == &func) {
    strIdx2Method.erase(func.GetBaseFuncNameWithTypeStrIdx());
  }
  for (auto it = methods.begin(); it != methods.end(); ++it) {
    if (*it == &func) {
      methods.erase(it--);
      return;
    }
  }
}

// This for class only, which only has 0 or 1 super class
Klass *Klass::GetSuperKlass() const {
  switch (superKlasses.size()) {
    case 0:
      return nullptr;
    case 1:
      return *superKlasses.begin();
    default:
      LogInfo::MapleLogger() << GetKlassName() << "\n";
      CHECK_FATAL(false, "GetSuperKlass expects less than one super class");
  }
}

bool Klass::IsKlassMethod(const MIRFunction *func) const {
  if (func == nullptr) {
    return false;
  }
  for (MIRFunction *method : methods) {
    if (method == func) {
      return true;
    }
  }
  return false;
}

bool Klass::ImplementsKlass() const {
  if (IsInterface()) {
    return false;
  }
  MIRClassType *classType = GetMIRClassType();
  ASSERT(classType != nullptr, "null ptr check");
  return (!classType->GetInterfaceImplemented().empty());
}

MapleVector<MIRFunction*> *Klass::GetCandidates(GStrIdx mnameNoklassStrIdx) const {
  auto it = strIdx2CandidateMap.find(mnameNoklassStrIdx);
  return ((it != strIdx2CandidateMap.end()) ? (it->second) : nullptr);
}

MIRFunction *Klass::GetUniqueMethod(GStrIdx mnameNoklassStrIdx) const {
  if (structType->IsIncomplete()) {
    return nullptr;
  }
  auto it = strIdx2CandidateMap.find(mnameNoklassStrIdx);
  if (it != strIdx2CandidateMap.end()) {
    MapleVector<MIRFunction*> *fv = it->second;
    if (fv != nullptr && fv->size() == 1) {
      return fv->at(0);
    }
  }
  return nullptr;
}

bool Klass::IsVirtualMethod(const MIRFunction &func) const {
  // May add other checking conditions in future
  return (func.GetAttr(FUNCATTR_virtual) && !func.GetAttr(FUNCATTR_private) && !func.GetAttr(FUNCATTR_abstract));
}

void Klass::CountVirtMethTopDown(const KlassHierarchy &kh) {
  MapleVector<MIRFunction*> *vec, *pvec;
  GStrIdx strIdx;
  auto *superAndImplClasses = alloc->GetMemPool()->New<MapleVector<Klass*>>(alloc->Adapter());
  // Add default methods of interface. Add them first because they have lowest
  // priorities compared with methods defined in classes
  for (TyIdx tyIdx : GetMIRClassType()->GetInterfaceImplemented()) {
    Klass *interface = kh.GetKlassFromTyIdx(tyIdx);
    if (interface != nullptr) {
      superAndImplClasses->push_back(interface);
    }
  }
  // Then add methods from superclasses
  for (Klass *superKlass : superKlasses) {
    superAndImplClasses->push_back(superKlass);
  }
  // Initialize strIdx2CandidateMap based on the superclass methods
  for (Klass *superAndImplClass : *superAndImplClasses) {
    ASSERT(superAndImplClass != nullptr, "Not a valid super class of interface");
    for (const auto &pair : superAndImplClass->strIdx2CandidateMap) {
      strIdx = pair.first;
      pvec = pair.second;
      ASSERT(pvec->size() == 1, "Expect exactly one method definition from parent class");
      if (strIdx2CandidateMap.find(strIdx) == strIdx2CandidateMap.end()) {
        vec = alloc->GetMemPool()->New<MapleVector<MIRFunction*>>(alloc->Adapter());
        vec->push_back(pvec->at(0));
        strIdx2CandidateMap[strIdx] = vec;
      } else {
        // Override the method coming from previous klass (must be an interface)
        ASSERT(strIdx2CandidateMap[strIdx]->size() == 1, "Expect exactly one method definition");
        ASSERT(kh.GetKlassFromStrIdx(strIdx2CandidateMap[strIdx]->at(0)->GetBaseClassNameStrIdx())->IsInterface(),
               "Override interface default methods");
        // Interfaces implemented methods override, need to determine the inherit relation.
        // class method can override interface method, interface method can override its parent's methods
        vec = strIdx2CandidateMap[strIdx];
        ASSERT(vec != nullptr, "nullptr check!");
        ASSERT(vec->size() == 1, "Expect exactly one method definition from parent class");
        Klass *parentKlass = kh.GetKlassFromFunc((*vec)[0]);
        Klass *childKlass = kh.GetKlassFromFunc((*pvec)[0]);
        CHECK_FATAL(childKlass != nullptr, "childKlass is null in Klass::CountVirtMethTopDown");
        if (childKlass->IsInterface() && !kh.IsSuperKlassForInterface(parentKlass, childKlass)) {
          continue;
        }
        (*vec)[0] = (*pvec)[0];
      }
    }
  }
  // Initialize mstridx2count_map based on the current class methods
  for (MIRFunction *method : methods) {
    ASSERT(method != nullptr, "null ptr check!");
    if (!IsVirtualMethod(*method)) {
      continue;
    }
    strIdx = method->GetBaseFuncNameWithTypeStrIdx();
    if (strIdx2CandidateMap.find(strIdx) != strIdx2CandidateMap.end()) {
      // Override the method coming from parent
      vec = strIdx2CandidateMap[strIdx];
      ASSERT(vec != nullptr, "nullptr check!");
      ASSERT(vec->size() == 1, "Expect exactly one method definition from parent class");
      (*vec)[0] = method;
    } else {
      // Newly declared and defined
      vec = alloc->GetMemPool()->New<MapleVector<MIRFunction*>>(alloc->Adapter());
      vec->push_back(method);
      strIdx2CandidateMap[strIdx] = vec;
    }
  }
}

void Klass::CountVirtMethBottomUp() {
  MapleVector<MIRFunction*> *vec;
  GStrIdx strIdx;
  for (Klass *subKlass : subKlasses) {
    CHECK_FATAL(subKlass != nullptr, "nullptr check failed");
    for (const auto &pair : subKlass->strIdx2CandidateMap) {
      strIdx = pair.first;
      if (strIdx2CandidateMap.find(strIdx) == strIdx2CandidateMap.end()) {
        continue;
      }
      vec = strIdx2CandidateMap[strIdx];
      MapleVector<MIRFunction*> *subv = pair.second;
      if (!vec->empty() && !subv->empty() && vec->at(0) == subv->at(0)) {
        // If this class and subclass share the same default implementation,
        // then we have to avoid duplicated counting.
        vec->insert(vec->end(), subv->begin() + 1, subv->end());
      } else {
        vec->insert(vec->end(), subv->begin(), subv->end());
      }
    }
  }
}

const MIRFunction *Klass::HasMethod(const std::string &funcname) const {
  for (auto *method : methods) {
    if (method->GetBaseFuncNameWithType().find(funcname) != std::string::npos) {
      return method;
    }
  }
  return nullptr;
}

Klass *KlassHierarchy::GetKlassFromStrIdx(GStrIdx strIdx) const {
  auto it = strIdx2KlassMap.find(strIdx);
  return ((it != strIdx2KlassMap.end()) ? (it->second) : nullptr);
}

Klass *KlassHierarchy::GetKlassFromTyIdx(TyIdx tyIdx) const {
  MIRType *type = GlobalTables::GetTypeTable().GetTypeFromTyIdx(tyIdx);
  return (type != nullptr ? GetKlassFromStrIdx(type->GetNameStrIdx()) : nullptr);
}

Klass *KlassHierarchy::GetKlassFromFunc(const MIRFunction *func) const {
  return (func != nullptr ? GetKlassFromStrIdx(func->GetBaseClassNameStrIdx()) : nullptr);
}

Klass *KlassHierarchy::GetKlassFromName(const std::string &name) const {
  return GetKlassFromStrIdx(GlobalTables::GetStrTable().GetStrIdxFromName(name));
}

Klass *KlassHierarchy::GetKlassFromLiteral(const std::string &name) const {
  return GetKlassFromStrIdx(GlobalTables::GetStrTable().GetStrIdxFromName(NameMangler::GetInternalNameLiteral(name)));
}


bool KlassHierarchy::IsSuperKlass(const Klass *super, const Klass *base) const {
  if (super == nullptr || base == nullptr || base->IsInterface()) {
    return false;
  }
  while (base != nullptr) {
    if (base == super) {
      return true;
    }
    base = base->GetSuperKlass();
  }
  return false;
}

// Interface
bool KlassHierarchy::IsSuperKlassForInterface(const Klass *super, const Klass *base) const {
  if (super == nullptr || base == nullptr) {
    return false;
  }
  if (!super->IsInterface() || !base->IsInterface()) {
    return false;
  }
  std::vector<const Klass*> tmpVector;
  tmpVector.push_back(base);
  for (size_t idx = 0; idx < tmpVector.size(); ++idx) {
    if (tmpVector[idx] == super) {
      return true;
    }
    for (const Klass *superKlass : tmpVector[idx]->GetSuperKlasses()) {
      tmpVector.push_back(superKlass);
    }
  }
  return false;
}

bool KlassHierarchy::IsInterfaceImplemented(Klass *interface, const Klass *base) const {
  if (interface == nullptr || base == nullptr) {
    return false;
  }
  if (!interface->IsInterface() || !base->IsClass()) {
    return false;
  }
  // All the implemented interfaces and their parent interfaces
  // are directly stored in this set, so no need to look up super
  return (base->GetImplInterfaces().find(interface) != base->GetImplInterfaces().end());
}

int KlassHierarchy::GetFieldIDOffsetBetweenClasses(const Klass &super, const Klass &base) const {
  int offset = 0;
  const Klass *superPtr = &super;
  const Klass *basePtr = &base;
  while (basePtr != superPtr) {
    basePtr = basePtr->GetSuperKlass();
    CHECK_FATAL(basePtr != nullptr, "null ptr check");
    offset++;
  }
  return offset;
}

bool KlassHierarchy::UpdateFieldID(TyIdx baseTypeIdx, TyIdx targetTypeIdx, FieldID &fldID) const {
  MIRType *baseType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(baseTypeIdx);
  MIRType *targetType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(targetTypeIdx);
  if (baseType->GetKind() == kTypePointer && targetType->GetKind() == kTypePointer) {
    baseType = static_cast<const MIRPtrType*>(baseType)->GetPointedType();
    targetType = static_cast<const MIRPtrType*>(targetType)->GetPointedType();
  }
  if (baseType->GetKind() != kTypeClass || targetType->GetKind() != kTypeClass) {
    return false;
  }
  Klass *baseKlass = GetKlassFromTyIdx(baseType->GetTypeIndex());
  ASSERT(baseKlass != nullptr, "null ptr check");
  Klass *targetKlass = GetKlassFromTyIdx(targetType->GetTypeIndex());
  ASSERT(targetKlass != nullptr, "null ptr check");
  if (IsSuperKlass(baseKlass, targetKlass)) {
    fldID += GetFieldIDOffsetBetweenClasses(*baseKlass, *targetKlass);
    return true;
  } else if (IsSuperKlass(targetKlass, baseKlass)) {
    fldID -= GetFieldIDOffsetBetweenClasses(*targetKlass, *baseKlass);
    return true;
  }
  return false;
}

bool KlassHierarchy::NeedClinitCheckRecursively(const Klass &kl) const {
  const Klass *klass = &kl;
  if (klass->IsClass()) {
    while (klass != nullptr) {
      if (klass->GetClinit()) {
        return true;
      }
      klass = klass->GetSuperKlass();
    }
    for (Klass *implInterface : kl.GetImplInterfaces()) {
      if (implInterface->GetClinit()) {
        for (auto &func : implInterface->GetMethods()) {
          if (!func->GetAttr(FUNCATTR_abstract) && !func->GetAttr(FUNCATTR_static)) {
            return true;
          }
        }
      }
    }
    return false;
  }
  if (klass->IsInterface()) {
    return klass->GetClinit();
  }
  return true;
}

// Get lowest common ancestor for two classes
Klass *KlassHierarchy::GetLCA(Klass *klass1, Klass *klass2) const {
  std::vector<Klass*> v1, v2;
  while (klass1 != nullptr) {
    v1.push_back(klass1);
    klass1 = klass1->GetSuperKlass();
  }
  while (klass2 != nullptr) {
    v2.push_back(klass2);
    klass2 = klass2->GetSuperKlass();
  }
  Klass *result = nullptr;
  size_t size1 = v1.size();
  size_t size2 = v2.size();
  size_t min = (size1 < size2) ? size1 : size2;
  for (size_t i = 1; i <= min; ++i) {
    if (v1[size1 - i] != v2[size2 - i]) {
      break;
    }
    result = v1[size1 - i];
  }
  return result;
}

TyIdx KlassHierarchy::GetLCA(TyIdx ty1, TyIdx ty2) const {
  Klass *result = GetLCA(GetKlassFromTyIdx(ty1), GetKlassFromTyIdx(ty2));
  return (result != nullptr ? result->GetTypeIdx() : TyIdx(0));
}

GStrIdx KlassHierarchy::GetLCA(GStrIdx str1, GStrIdx str2) const {
  Klass *result = GetLCA(GetKlassFromStrIdx(str1), GetKlassFromStrIdx(str2));
  return (result != nullptr ? result->GetKlassNameStrIdx() : GStrIdx(0));
}

const std::string &KlassHierarchy::GetLCA(const std::string &name1, const std::string &name2) const {
  Klass *result = GetLCA(GetKlassFromName(name1), GetKlassFromName(name2));
  return (result != nullptr ? result->GetKlassName() : GlobalTables::GetStrTable().GetStringFromStrIdx(GStrIdx(0)));
}

void KlassHierarchy::AddKlasses() {
  for (MIRType *type : GlobalTables::GetTypeTable().GetTypeTable()) {
    if (type == nullptr || (type->GetKind() != kTypeClass && type->GetKind() != kTypeInterface)) {
      continue;
    }
    GStrIdx strIdx = type->GetNameStrIdx();
    Klass *klass = GetKlassFromStrIdx(strIdx);
    if (klass != nullptr) {
      if (klass->GetKlassName().compare(NameMangler::kThrowClassStr) == 0) {
        klass->SetExceptionKlass();
      }
      continue;
    }
    auto *stype = static_cast<MIRStructType*>(type);
    klass = GetMempool()->New<Klass>(stype, &alloc);
    strIdx2KlassMap[strIdx] = klass;
  }
}

void KlassHierarchy::ExceptionFlagProp(Klass &klass) {
  klass.SetExceptionKlass();
  for (Klass *subClass : klass.GetSubKlasses()) {
    ASSERT(subClass != nullptr, "null ptr check!");
    subClass->SetExceptionKlass();
    ExceptionFlagProp(*subClass);
  }
}

void KlassHierarchy::AddKlassRelationAndMethods() {
  for (const auto &pair : strIdx2KlassMap) {
    Klass *klass = pair.second;
    ASSERT(klass, "null ptr check");
    Klass *superKlass = nullptr;
    if (klass->IsInterface() || klass->IsInterfaceIncomplete()) {
      MIRInterfaceType *itype = klass->GetMIRInterfaceType();
      ASSERT(itype != nullptr, "null ptr check");
      // Java interface supports multiple inheritance
      for (TyIdx superTyIdx : itype->GetParentsTyIdx()) {
        superKlass = GetKlassFromTyIdx(superTyIdx);
        if (superKlass != nullptr) {
          klass->AddSuperKlass(superKlass);
          superKlass->AddSubKlass(klass);
        }
      }
    } else if (klass->IsClass() || klass->IsClassIncomplete()) {
      // Class
      MIRClassType *classType = klass->GetMIRClassType();
      ASSERT(classType != nullptr, "null ptr check");
      // Add interface relationship
      for (TyIdx intfTyIdx : classType->GetInterfaceImplemented()) {
        Klass *intfKlass = GetKlassFromTyIdx(intfTyIdx);
        if (intfKlass != nullptr) {
          intfKlass->AddImplKlass(klass);
          klass->AddImplInterface(intfKlass);
        }
      }
      superKlass = GetKlassFromTyIdx(classType->GetParentTyIdx());
      // Add superclass/subclass for each class.
      if (superKlass != nullptr) {
        klass->AddSuperKlass(superKlass);
        superKlass->AddSubKlass(klass);
        // klass implements same interfaces inherited from its parent
        for (Klass *intfklass : superKlass->GetImplInterfaces()) {
          intfklass->AddImplKlass(klass);
          klass->AddImplInterface(intfklass);
        }
      }
    }
    // Add method info
    for (const auto &methodPair : klass->GetMIRStructType()->GetMethods()) {
      MIRSymbol *funcSym = GlobalTables::GetGsymTable().GetSymbolFromStidx(methodPair.first.Idx());
      MIRFunction *method = funcSym->GetFunction();
      klass->AddMethod(method);
      if (method->GetName().compare(klass->GetKlassName() + NameMangler::kClinitSuffix) == 0) {
        klass->SetClinit(method);
      }
    }
  }
  // Propagate isExceptionKlass flag
  if (GetKlassFromLiteral(NameMangler::kThrowClassStr) != nullptr) {
    ExceptionFlagProp(*GetKlassFromLiteral(NameMangler::kThrowClassStr));
    CHECK_FATAL(GetKlassFromLiteral(NameMangler::kThrowClassStr)->IsExceptionKlass(), "must be exception class");
  }
  if (GetKlassFromLiteral(kJavaLangNoMethodStr) != nullptr) {
    CHECK_FATAL(GetKlassFromLiteral(kJavaLangNoMethodStr)->IsExceptionKlass(), "must be exception class");
  }
}

void KlassHierarchy::TagThrowableKlasses() {
  Klass *throwable = GetKlassFromName(NameMangler::kThrowClassStr);
  if (throwable == nullptr) {
    return;
  }
  for (const auto &pair : strIdx2KlassMap) {
    Klass *klass = pair.second;
    ASSERT(klass != nullptr, "unexpeced null klass");
    if (!klass->IsInterface() && IsSuperKlass(throwable, klass)) {
      klass->SetExceptionKlass();
    }
  }
}

static void CollectImplInterfaces(const Klass &klass, std::set<Klass*> &implInterfaceSet) {
  for (Klass *superKlass : klass.GetSuperKlasses()) {
    if (implInterfaceSet.find(superKlass) == implInterfaceSet.end()) {
      ASSERT(superKlass != nullptr, "null ptr check!");
      if (superKlass->IsInterface()) {
        implInterfaceSet.insert(superKlass);
      }
      CollectImplInterfaces(*superKlass, implInterfaceSet);
    }
  }
  for (Klass *interfaceKlass : klass.GetImplInterfaces()) {
    if (implInterfaceSet.find(interfaceKlass) == implInterfaceSet.end()) {
      implInterfaceSet.insert(interfaceKlass);
      ASSERT(interfaceKlass != nullptr, "null ptr check!");
      CollectImplInterfaces(*interfaceKlass, implInterfaceSet);
    }
  }
}

void KlassHierarchy::UpdateImplementedInterfaces() {
  for (const auto &pair : strIdx2KlassMap) {
    Klass *klass = pair.second;
    ASSERT(klass != nullptr, "null ptr check");
    if (!klass->IsInterface()) {
      std::set<Klass*> implInterfaceSet;
      CollectImplInterfaces(*klass, implInterfaceSet);
      for (auto interface : implInterfaceSet) {
        // Add missing parent interface to class link
        interface->AddImplKlass(klass);
        klass->AddImplInterface(interface);
      }
    }
  }
}

void KlassHierarchy::GetParentKlasses(const Klass &klass, std::vector<Klass*> &parentKlasses) const {
  for (Klass *superKlass : klass.GetSuperKlasses()) {
    parentKlasses.push_back(superKlass);
  }
  if (!klass.IsInterface()) {
    for (Klass *iklass : klass.GetImplInterfaces()) {
      parentKlasses.push_back(iklass);
    }
  }
}

void KlassHierarchy::GetChildKlasses(const Klass &klass, std::vector<Klass*> &childKlasses) const {
  for (Klass *subKlass : klass.GetSubKlasses()) {
    childKlasses.push_back(subKlass);
  }
  if (klass.IsInterface()) {
    for (Klass *implKlass : klass.GetImplKlasses()) {
      childKlasses.push_back(implKlass);
    }
  }
}

void KlassHierarchy::TopologicalSortKlasses() {
  std::set<Klass*> inQueue;  // Local variable, no need to use MapleSet
  for (const auto &pair : strIdx2KlassMap) {
    Klass *klass = pair.second;
    ASSERT(klass != nullptr, "klass can not be nullptr");
    if (!klass->HasSuperKlass() && !klass->ImplementsKlass()) {
      topoWorkList.push_back(klass);
      inQueue.insert(klass);
    }
  }
  // Top-down iterates all nodes
  for (size_t i = 0; i < topoWorkList.size(); ++i) {
    Klass *klass = topoWorkList[i];
    std::vector<Klass*> childklasses;
    ASSERT(klass != nullptr, "null ptr check!");
    GetChildKlasses(*klass, childklasses);
    for (Klass *childklass : childklasses) {
      if (inQueue.find(childklass) == inQueue.end()) {
        // callee has not been visited
        bool parentKlassAllVisited = true;
        std::vector<Klass*> parentKlasses;
        ASSERT(childklass != nullptr, "null ptr check!");
        GetParentKlasses(*childklass, parentKlasses);
        // Check whether all callers of the current callee have been visited
        for (Klass *parentKlass : parentKlasses) {
          if (inQueue.find(parentKlass) == inQueue.end()) {
            parentKlassAllVisited = false;
            break;
          }
        }
        if (parentKlassAllVisited) {
          topoWorkList.push_back(childklass);
          inQueue.insert(childklass);
        }
      }
    }
  }
}

void KlassHierarchy::CountVirtualMethods() const {
  // Top-down iterates all klass nodes
  for (Klass *klass : topoWorkList) {
    klass->CountVirtMethTopDown(*this);
  }
  // Bottom-up iterates all klass nodes
  for (size_t i = topoWorkList.size(); i != 0; --i) {
    topoWorkList[i - 1]->CountVirtMethBottomUp();
  }
}

Klass *KlassHierarchy::AddClassFlag(const std::string &name, uint32 flag) {
  Klass *refKlass = GetKlassFromLiteral(name);
  if (refKlass != nullptr) {
    refKlass->SetFlag(flag);
  }
  return refKlass;
}

// Mark klasses those implement the finalize method, or inherit
// from super klass (except java.lang.Object).
// Mark klasses those or superclasses are references.
void KlassHierarchy::MarkClassFlags() {
  Klass *cleanerKlass = AddClassFlag("Lsun_2Fmisc_2FCleaner_3B", kClassCleaner);
  AddClassFlag("Ljava_2Flang_2Fref_2FSoftReference_3B", kClassSoftreference);
  AddClassFlag("Ljava_2Flang_2Fref_2FWeakReference_3B", kClassWeakreference);
  AddClassFlag("Ljava_2Flang_2Fref_2FPhantomReference_3B", kClassPhantomreference);
  AddClassFlag("Ljava_2Flang_2Fref_2FFinalizerReference_3B", kClassFinalizereference);
  AddClassFlag("Ljava_2Flang_2Fref_2FFinalizerReference_24Sentinel_3B", kClassFinalizerreferenceSentinel);
  Klass *klassObject = GetKlassFromLiteral(NameMangler::kJavaLangObjectStr);
  GStrIdx finalize = GlobalTables::GetStrTable().GetOrCreateStrIdxFromName("finalize_7C_28_29V");
  for (Klass *klass : topoWorkList) {
    if (klass == klassObject) {
      continue;
    }
    if (klass->IsInterface()) {
      continue;
    }
    Klass *super = klass->GetSuperKlass();
    // Mark Reference
    // sun.misc.Cleaner's superclass is PhantomReference.
    // Break this chain to process Cleaner correctly.
    if (super != nullptr && super->IsReference() && klass != cleanerKlass) {
      klass->SetFlag(super->GetFlag(kClassReference));
    }
    // Mark Finalizer
    if (super != nullptr && super->HasFinalizer()) {
      klass->SetHasFinalizer();
    } else {
      for (auto &method : klass->GetMethods()) {
        if (method->GetBaseFuncNameWithTypeStrIdx() == finalize) {
          klass->SetHasFinalizer();
          break;
        }
      }
    }
    // Mark native function info
    for (auto &method : klass->GetMethods()) {
      if (method->GetAttr(FUNCATTR_native)) {
        klass->SetHasNativeMethod(true);
        break;
      }
    }
  }
}

void KlassHierarchy::Dump() const {
  for (Klass *klass : topoWorkList) {
    klass->Dump();
  }
}


void KlassHierarchy::BuildHierarchy() {
  // Scan class list and generate Klass without method information
  AddKlasses();
  // Fill class method info. Connect superclass<->subclass and
  // interface->implementation edges.
  AddKlassRelationAndMethods();
  TagThrowableKlasses();
  // In the case of "class C implements B; interface B extends A;",
  // we need to add a link between C and A.
  UpdateImplementedInterfaces();
  TopologicalSortKlasses();
  MarkClassFlags();
  if (!strIdx2KlassMap.empty()) {
    WKTypes::Init();
  }
}

KlassHierarchy::KlassHierarchy(MIRModule *mirmodule, MemPool *memPool)
    : AnalysisResult(memPool),
      alloc(memPool),
      mirModule(mirmodule),
      strIdx2KlassMap(std::less<GStrIdx>(), alloc.Adapter()),
      topoWorkList(alloc.Adapter()) {}

AnalysisResult *DoKlassHierarchy::Run(MIRModule *module, ModuleResultMgr *m) {
  MemPool *memPool = memPoolCtrler.NewMemPool("classhierarchy mempool");
  KlassHierarchy *kh = memPool->New<KlassHierarchy>(module, memPool);
  KlassHierarchy::traceFlag = TRACE_PHASE;
  kh->BuildHierarchy();
#if MIR_JAVA
  if (!Options::skipVirtualMethod) {
    kh->CountVirtualMethods();
  }
#else
  kh->CountVirtualMethods();
#endif
  if (KlassHierarchy::traceFlag) {
    kh->Dump();
  }
  m->AddResult(GetPhaseID(), *module, *kh);
  return kh;
}

MIRType *WKTypes::javaLangObject;
MIRType *WKTypes::javaLangString;
MIRType *WKTypes::javaLangObjectSerializable;
MIRType *WKTypes::javaLangComparable;
MIRType *WKTypes::javaLangCharSequence;
MIRType *WKTypes::javaLangClass;
MIRType *WKTypes::javaLangRefGenericDeclaration;
MIRType *WKTypes::javaLangRefAnnotatedElement;
MIRType *WKTypes::javaLangRefType;
MIRType *WKTypes::javaLangRefMethod;
MIRType *WKTypes::javaLangRefExecutable;
MIRType *WKTypes::javaLangRefAccessibleObject;
MIRType *WKTypes::javaLangRefMember;
MIRType *WKTypes::javaLangRefField;
MIRType *WKTypes::javaLangRefConstructor;
inline static MIRType *GetMIRTypeFromName(const std::string &name) {
  GStrIdx gStrIdx = GlobalTables::GetStrTable().GetStrIdxFromName(NameMangler::GetInternalNameLiteral(name));
  return GlobalTables::GetTypeTable().GetTypeFromTyIdx(GlobalTables::GetTypeNameTable().GetTyIdxFromGStrIdx(gStrIdx));
}

void WKTypes::Init() {
  javaLangObject = GetMIRTypeFromName(NameMangler::kJavaLangObjectStr);
  javaLangString = GetMIRTypeFromName("Ljava_2Flang_2FString_3B");
  javaLangObjectSerializable = GetMIRTypeFromName("Ljava_2Fio_2FSerializable_3B");
  javaLangComparable = GetMIRTypeFromName("Ljava_2Flang_2FComparable_3B");
  javaLangCharSequence = GetMIRTypeFromName("Ljava_2Flang_2FCharSequence_3B");
  javaLangClass = GetMIRTypeFromName("Ljava_2Flang_2FClass_3B");
  javaLangRefGenericDeclaration = GetMIRTypeFromName("Ljava_2Flang_2Freflect_2FGenericDeclaration_3B");
  javaLangRefAnnotatedElement = GetMIRTypeFromName("Ljava_2Flang_2Freflect_2FAnnotatedElement_3B");
  javaLangRefType = GetMIRTypeFromName("Ljava_2Flang_2Freflect_2FType_3B");
  javaLangRefMethod = GetMIRTypeFromName("Ljava_2Flang_2Freflect_2FMethod_3B");
  javaLangRefExecutable = GetMIRTypeFromName("Ljava_2Flang_2Freflect_2FExecutable_3B");
  javaLangRefAccessibleObject = GetMIRTypeFromName("Ljava_2Flang_2Freflect_2FAccessibleObject_3B");
  javaLangRefMember = GetMIRTypeFromName("Ljava_2Flang_2Freflect_2FMember_3B");
  javaLangRefField = GetMIRTypeFromName("Ljava_2Flang_2Freflect_2FField_3B");
  javaLangRefConstructor = GetMIRTypeFromName("Ljava_2Flang_2Freflect_2FConstructor_3B");
}

// Return true if node n may point to a String object.
// String is declared as:
//   public final class String implements java.io.Serializable, Comparable<String>, CharSequence
// So n can point to String only if n's type is a ref to String or Object, or is an interface
// type of java.io.Serializable, Comparable or CharSequence
bool WKTypes::Util::MayRefString(const BaseNode &n, MIRType &type) {
  if ((n.GetPrimType() == PTY_ref || n.GetPrimType() == PTY_ptr) && type.GetKind() == kTypePointer) {
    auto *pointType = static_cast<MIRPtrType*>(&type);
    MIRType *pointedType = pointType->GetPointedType();
    if (pointedType == javaLangString || pointedType == javaLangObjectSerializable ||
        pointedType == javaLangComparable ||
        pointedType == javaLangCharSequence || pointedType == javaLangObject) {
      return true;
    }
  }
  return false;
}

// Return true if node n may point to Meta object, i.e, n is a reference
// of java.lang.Class, java.lang.reflect.Method, java.lang.reflect.Field,
// java.lang.reflect.Constructor
bool WKTypes::Util::MayRefMeta(const BaseNode &n, MIRType &type) {
  if ((n.GetPrimType() == PTY_ref || n.GetPrimType() == PTY_ptr) && type.GetKind() == kTypePointer) {
    auto *pointType = static_cast<MIRPtrType*>(&type);
    MIRType *pointedType = pointType->GetPointedType();
    // Definition of java.lang.Class:
    // public final class Class<T> implements java.io.Serializable,
    // GenericDeclaration,
    // Type,
    // AnnotatedElement {...}
    // public interface Serializable {}
    // public interface GenericDeclaration extends AnnotatedElement {...}
    // public interface AnnotatedElement {...}
    // public interface Type {...}
    // public interface AnnotatedElement {...}
    if (pointedType == javaLangClass || pointedType == javaLangObjectSerializable ||
        pointedType == javaLangRefGenericDeclaration || pointedType == javaLangRefAnnotatedElement ||
        pointedType == javaLangRefType || pointedType == javaLangObject) {
      return true;
    }
    // Definition of java.lang.reflect.Method:
    // public final class Method extends Executable {...}
    // public abstract class Executable extends AccessibleObject
    // implements Member, GenericDeclaration {...}
    // public class AccessibleObject implements AnnotatedElement {...}
    // public interface AnnotatedElement {...}
    // public interface Member {...}
    // public interface GenericDeclaration extends AnnotatedElement {...}
    // public interface AnnotatedElement {...}
    if (pointedType == javaLangRefMethod || pointedType == javaLangRefExecutable ||
        pointedType == javaLangRefAccessibleObject ||
        pointedType == javaLangRefMember || pointedType == javaLangRefGenericDeclaration ||
        pointedType == javaLangRefAnnotatedElement || pointedType == javaLangObject) {
      return true;
    }
    // Definition of java.lang.reflect.Field:
    // public final class Field extends AccessibleObject implements Member {...}
    if (pointedType == javaLangRefField) {  // super classes/interfaces are checked by javaLangRefMethod
      return true;
    }
    // Definition of java.lang.reflect.Constructor:
    // public final class Constructor<T> extends Executable {...}
    if (pointedType == javaLangRefConstructor) {  // super classes are checked by javaLangRefMethod
      return true;
    }
  }
  return false;
}

// Return true if node 'n', whose type is 'type', can not form a reference cycle.
// E.g, all fields are primitive types, String, array of primitive, etc.
bool WKTypes::Util::MayNotRefCyclicly(const BaseNode &n, MIRType &type) {
  CHECK_FATAL((n.GetPrimType() == PTY_ref || n.GetPrimType() == PTY_ptr) && type.GetKind() == kTypePointer,
              "n must be a reference");
  std::set<MIRType*> workList;
  workList.insert(&type);
  return NotCyclicType(type, workList);
}

// Helper function of WKTypes::Util::MayNotRefCyclicly.
bool WKTypes::Util::NotCyclicType(MIRType &type, std::set<MIRType*> &workList) {
  // Fast path for special cases: String, Class
  if (&type == javaLangString || &type == javaLangClass) {
    return true;
  }
  if (type.GetKind() == kTypeScalar) {  // primitive type
    return true;
  }
  if (type.GetKind() == kTypePointer) {
    auto *pointedType = static_cast<MIRPtrType*>(&type)->GetPointedType();
    ASSERT(pointedType != nullptr, "null ptr check!");
    return NotCyclicType(*pointedType, workList);
  }
  if (type.GetKind() == kTypeJArray) {
    MIRType *elemType =
        GlobalTables::GetTypeTable().GetTypeFromTyIdx(static_cast<MIRJarrayType*>(&type)->GetElemTyIdx());
    ASSERT(elemType != nullptr, "null ptr check!");
    return NotCyclicType(*elemType, workList);
  }
  if (type.GetKind() == kTypeClass) {
    auto *classType = static_cast<MIRClassType*>(&type);
    if (!classType->IsFinal()) {  // Not sure what actual class it refers to
      return false;
    }
    std::vector<FieldPair> allFields;
    allFields.insert(allFields.end(), classType->GetFields().begin(), classType->GetFields().end());
    // ignore static fields for now, they are not recycled anyway
    MIRType *parentType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(classType->GetParentTyIdx());
    while (parentType != nullptr) {
      CHECK_FATAL(parentType->GetKind() == kTypeClass, "parent class");
      auto *parentclass = static_cast<MIRClassType*>(parentType);
      allFields.insert(allFields.end(), parentclass->GetFields().begin(), parentclass->GetFields().end());
      // ignore static fields for now, they are not recycled anyway
      parentType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(parentclass->GetParentTyIdx());
    }
    // Ignore interface fields, as they are all static final (constant) variables
    for (FieldPair &fieldPair : allFields) {
      MIRType *fieldType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(fieldPair.second.first);
      ASSERT(fieldType != nullptr, "null ptr check!");
      if (fieldType->GetKind() == kTypePointer) {
        // Strictly, we should check whether fieldType is 'assignable' from any type
        // recorded in worklist, which means a cycle may be formed. However, this
        // function returns false for any non-final calss (see above), so we only
        // check if fieldType is 'equal' to any type recorded in the worklist.
        // An example:
        // class B extends A {
        //    A field_a;
        // }
        // field_a may point to an instance of B and henceforce a possible cycle.
        // Since A is not a final class, we can recognize it as a cycle candidate.
        if (workList.find(fieldType) != workList.end()) {
          return false;
        }
        workList.insert(fieldType);
      }
      if (!NotCyclicType(*fieldType, workList)) {
        return false;
      }
    }
    return true;
  }
  if (type.GetKind() == kTypeInterface) {
    // Perhaps something more can do.
    return false;
  }
  return false;
}
}  // namespace maple
