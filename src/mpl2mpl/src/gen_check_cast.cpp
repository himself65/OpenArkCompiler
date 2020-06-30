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
#include "gen_check_cast.h"
#include <iostream>
#include <algorithm>
#include "reflection_analysis.h"
#include "mir_lower.h"

namespace {
constexpr char kMCCReflectThrowCastException[] = "MCC_Reflect_ThrowCastException";
constexpr char kMCCReflectCheckCastingNoArray[] = "MCC_Reflect_Check_Casting_NoArray";
constexpr char kMCCReflectCheckCastingArray[] = "MCC_Reflect_Check_Casting_Array";
constexpr char kCastTargetClass[] = "castTargetClass";
} // namespace

// This phase does two things:
// #1 implement the opcode check-cast vx, type_id
//    according to the check-cast definition:
//    Checks whether the object reference in vx can be cast
//    to an instance of a class referenced by type_id.
//    Throws ClassCastException if the cast is not possible, continues execution otherwise.
//    in our case check if object can be cast or insert MCC_Reflect_ThrowCastException
//    before the stmt.
// #2 optimise instance-of && cast
namespace maple {
CheckCastGenerator::CheckCastGenerator(MIRModule &mod, KlassHierarchy *kh, bool dump)
    : FuncOptimizeImpl(mod, kh, dump) {
  InitTypes();
  InitFuncs();
}


void CheckCastGenerator::InitTypes() {
  const MIRType *javaLangObjectType = WKTypes::Util::GetJavaLangObjectType();
  CHECK_FATAL(javaLangObjectType != nullptr, "The pointerObjType in InitTypes is null!");
  pointerObjType = GlobalTables::GetTypeTable().GetOrCreatePointerType(*javaLangObjectType);
  classinfoType = GlobalTables::GetTypeTable().GetOrCreateClassType(namemangler::kClassMetadataTypeName,
                                                                    GetMIRModule());
  pointerClassMetaType = GlobalTables::GetTypeTable().GetOrCreatePointerType(*classinfoType);
}

void CheckCastGenerator::InitFuncs() {
  throwCastException = builder->GetOrCreateFunction(kMCCReflectThrowCastException, TyIdx(PTY_void));
  throwCastException->SetAttr(FUNCATTR_nosideeffect);
  checkCastingNoArray = builder->GetOrCreateFunction(kMCCReflectCheckCastingNoArray, TyIdx(PTY_void));
  checkCastingNoArray->SetAttr(FUNCATTR_nosideeffect);
  checkCastingArray = builder->GetOrCreateFunction(kMCCReflectCheckCastingArray, TyIdx(PTY_void));
  checkCastingArray->SetAttr(FUNCATTR_nosideeffect);

}

MIRSymbol *CheckCastGenerator::GetOrCreateClassInfoSymbol(const std::string &className) {
  std::string classInfoName = CLASSINFO_PREFIX_STR + className;
  MIRSymbol *classInfoSymbol = builder->GetGlobalDecl(classInfoName);
  if (classInfoSymbol == nullptr) {
    GStrIdx gStrIdx = GlobalTables::GetStrTable().GetStrIdxFromName(className);
    MIRType *classType =
        GlobalTables::GetTypeTable().GetTypeFromTyIdx(GlobalTables::GetTypeNameTable().GetTyIdxFromGStrIdx(gStrIdx));
    MIRStorageClass sclass = (classType != nullptr && static_cast<MIRClassType*>(classType)->IsLocal()) ? kScGlobal
                                                                                                        : kScExtern;
    // Creating global symbol needs synchronization.
    classInfoSymbol = builder->CreateGlobalDecl(classInfoName, *GlobalTables::GetTypeTable().GetPtr(), sclass);
  }
  return classInfoSymbol;
}

void CheckCastGenerator::GenCheckCast(StmtNode &stmt) {
  // Handle the special case like (Type)null, we don't need a checkcast.
  if (stmt.GetOpCode() == OP_intrinsiccallwithtypeassigned) {
    auto *callNode = static_cast<IntrinsiccallNode*>(&stmt);
    ASSERT(callNode->GetNopndSize() == 1, "array size error");
    BaseNode *opnd = callNode->Opnd(0);
    if (opnd->GetOpCode() == OP_constval) {
      const size_t callNodeNretsSize = callNode->GetReturnVec().size();
      CHECK_FATAL(callNodeNretsSize > 0, "container check");
      CallReturnPair callReturnPair = callNode->GetReturnVec()[0];
      StmtNode *assignReturnTypeNode = nullptr;
      if (!callReturnPair.second.IsReg()) {
        assignReturnTypeNode =
            builder->CreateStmtDassign(callReturnPair.first, callReturnPair.second.GetFieldID(), opnd);
      } else {
        PregIdx pregIdx = callReturnPair.second.GetPregIdx();
        MIRPreg *mirPreg = currFunc->GetPregTab()->PregFromPregIdx(pregIdx);
        assignReturnTypeNode = builder->CreateStmtRegassign(mirPreg->GetPrimType(), pregIdx, opnd);
      }
      currFunc->GetBody()->ReplaceStmt1WithStmt2(&stmt, assignReturnTypeNode);
      return;
    }
  }
  // Do type check first.
  auto *callNode = static_cast<IntrinsiccallNode*>(&stmt);
  ASSERT(callNode->GetNopndSize() == 1, "array size error");
  TyIdx checkTyidx = callNode->GetTyIdx();
  MIRType *checkType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(checkTyidx);
  Klass *checkKlass = klassHierarchy->GetKlassFromTyIdx(static_cast<MIRPtrType*>(checkType)->GetPointedTyIdx());

  {
    if ((checkKlass != nullptr) && (checkKlass->GetKlassName() != "")) {
      if (checkKlass->GetKlassName() == namemangler::kJavaLangObjectStr) {
        const size_t callNodeNopndSize1 = callNode->GetNopndSize();
        CHECK_FATAL(callNodeNopndSize1 > 0, "container check");
        if (callNode->GetNopndAt(0)->GetPrimType() != PTY_ref && callNode->GetNopndAt(0)->GetPrimType() != PTY_ptr) {
          // If source = Ljava_2Flang_2FObject_3B, sub = primitive type then throw CastException.
          MIRSymbol *classSt = GetOrCreateClassInfoSymbol(checkKlass->GetKlassName());
          BaseNode *valueExpr = builder->CreateExprAddrof(0, *classSt);
          MapleVector<BaseNode*> args(builder->GetCurrentFuncCodeMpAllocator()->Adapter());
          args.push_back(valueExpr);
          args.push_back(callNode->GetNopndAt(0));
          args.push_back(builder->CreateIntConst(0, PTY_ptr));
          StmtNode *dassignStmt = builder->CreateStmtCall(throwCastException->GetPuidx(), args);
          currFunc->GetBody()->InsertBefore(&stmt, dassignStmt);
        }
      } else {
        MIRSymbol *classSt = GetOrCreateClassInfoSymbol(checkKlass->GetKlassName());
        BaseNode *valueExpr = builder->CreateExprAddrof(0, *classSt);

        BaseNode *castClassReadNode = nullptr;
        StmtNode *castClassAssign = nullptr;
        BaseNode *opnd0 = callNode->GetNopndAt(0);
        if ((opnd0 != nullptr) && (opnd0->GetOpCode() == OP_regread)) {
          PregIdx castClassSymPregIdx = currFunc->GetPregTab()->CreatePreg(PTY_ref);
          castClassAssign = builder->CreateStmtRegassign(PTY_ref, castClassSymPregIdx, valueExpr);
          castClassReadNode = builder->CreateExprRegread(PTY_ref, castClassSymPregIdx);
        } else {
          MIRSymbol *castClassSym =
              builder->GetOrCreateLocalDecl(kCastTargetClass, *GlobalTables::GetTypeTable().GetRef());
          castClassAssign = builder->CreateStmtDassign(*castClassSym, 0, valueExpr);
          castClassReadNode = builder->CreateExprDread(*castClassSym);
        }
        BaseNode *nullPtrConst = builder->CreateIntConst(0, PTY_ptr);
        const size_t callNodeNopndSize2 = callNode->GetNopndSize();
        CHECK_FATAL(callNodeNopndSize2 > 0, "container check");
        BaseNode *cond =
            builder->CreateExprCompare(OP_ne, *GlobalTables::GetTypeTable().GetUInt1(),
                                       *GlobalTables::GetTypeTable().GetPtrType(), callNode->GetNopndAt(0),
                                       nullPtrConst);
        auto *ifStmt = static_cast<IfStmtNode*>(builder->CreateStmtIf(cond));
        MIRType *mVoidPtr = GlobalTables::GetTypeTable().GetVoidPtr();
        CHECK_FATAL(mVoidPtr != nullptr, "builder->GetVoidPtr() is null in CheckCastGenerator::GenCheckCast");
        BaseNode *opnd = callNode->GetNopndAt(0);
        BaseNode *ireadExpr = GetObjectShadow(opnd);
        BaseNode *innerCond = builder->CreateExprCompare(OP_ne, *GlobalTables::GetTypeTable().GetUInt1(),
                                                         *GlobalTables::GetTypeTable().GetPtrType(), castClassReadNode,
                                                         ireadExpr);
        auto *innerIfStmt = static_cast<IfStmtNode*>(builder->CreateStmtIf(innerCond));
        MapleVector<BaseNode*> args(builder->GetCurrentFuncCodeMpAllocator()->Adapter());
        args.push_back(castClassReadNode);
        args.push_back(opnd);
        StmtNode *dassignStmt = builder->CreateStmtCall(checkCastingNoArray->GetPuidx(), args);
        innerIfStmt->GetThenPart()->AddStatement(dassignStmt);
        ifStmt->GetThenPart()->AddStatement(castClassAssign);
        ifStmt->GetThenPart()->AddStatement(innerIfStmt);
        currFunc->GetBody()->InsertBefore(&stmt, ifStmt);
      }
    } else {
      MIRType *pointedType =
          GlobalTables::GetTypeTable().GetTypeFromTyIdx(static_cast<MIRPtrType*>(checkType)->GetPointedTyIdx());
      if (pointedType->GetKind() == kTypeJArray) {
        // Java array.
        auto *jarrayType = static_cast<MIRJarrayType*>(pointedType);
        std::string arrayName = jarrayType->GetJavaName();
        int dim = 0;
        while (arrayName[dim] == 'A') {
          ++dim;
        }
        MIRSymbol *elemClassSt = nullptr;
        std::string elementName = arrayName.substr(dim, arrayName.size() - dim);
        MIRType *mVoidPtr2 = GlobalTables::GetTypeTable().GetVoidPtr();
        CHECK_FATAL(mVoidPtr2 != nullptr, "null ptr check");
        if (elementName == "I" || elementName == "F" || elementName == "B" || elementName == "C" ||
            elementName == "S" || elementName == "J" || elementName == "D" || elementName == "Z" ||
            elementName == "V") {
          std::string primClassinfoName = PRIMITIVECLASSINFO_PREFIX_STR + elementName;
          elemClassSt = builder->GetGlobalDecl(primClassinfoName);
          if (elemClassSt == nullptr) {
            elemClassSt = builder->CreateGlobalDecl(primClassinfoName, *GlobalTables::GetTypeTable().GetPtr());
          }
        } else {
          elemClassSt = GetOrCreateClassInfoSymbol(elementName);
        }
        BaseNode *valueExpr = builder->CreateExprAddrof(0, *elemClassSt);
        MapleVector<BaseNode*> opnds(currFunc->GetCodeMempoolAllocator().Adapter());
        opnds.push_back(valueExpr);
        const size_t callNodeNopndSize3 = callNode->GetNopndSize();
        CHECK_FATAL(callNodeNopndSize3 > 0, "container check");
        opnds.push_back(callNode->GetNopndAt(0));
        opnds.push_back(builder->CreateIntConst(dim, PTY_ptr));
        StmtNode *dassignStmt = builder->CreateStmtCall(checkCastingArray->GetPuidx(), opnds);
        currFunc->GetBody()->InsertBefore(&stmt, dassignStmt);
      } else {
        MIRTypeKind kd = pointedType->GetKind();
        if (kd == kTypeStructIncomplete || kd == kTypeClassIncomplete || kd == kTypeInterfaceIncomplete) {
          LogInfo::MapleLogger() << "Warining: CheckCastGenerator::GenCheckCast "
                                 << GlobalTables::GetStrTable().GetStringFromStrIdx(pointedType->GetNameStrIdx())
                                 << " INCOMPLETE \n";
        } else {
          CHECK_FATAL(false, "unsupport kind");
        }
      }
    }
  }
  if (callNode->GetOpCode() == OP_intrinsiccallwithtype) {
    return;
  }
  BaseNode *opnd = callNode->Opnd(0);
  ASSERT(opnd->GetOpCode() == OP_dread || opnd->GetOpCode() == OP_regread || opnd->GetOpCode() == OP_iread ||
         opnd->GetOpCode() == OP_retype, "unknown calltype! check it!");
  MIRType *fromType = nullptr;
  if (opnd->GetOpCode() == OP_dread) {
    fromType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(
        currFunc->GetLocalOrGlobalSymbol(static_cast<AddrofNode*>(opnd)->GetStIdx())->GetTyIdx());
  } else if (opnd->GetOpCode() == OP_retype) {
    fromType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(static_cast<RetypeNode*>(opnd)->GetTyIdx());
  } else if (opnd->GetOpCode() == OP_iread) {
    auto *irnode = static_cast<IreadNode*>(opnd);
    auto *ptrType = static_cast<MIRPtrType*>(GlobalTables::GetTypeTable().GetTypeFromTyIdx(irnode->GetTyIdx()));
    if (irnode->GetFieldID() != 0) {
      MIRType *pointedType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(ptrType->GetPointedTyIdx());
      MIRStructType *structType = nullptr;
      if (pointedType->GetKind() != kTypeJArray) {
        structType = static_cast<MIRStructType*>(pointedType);
      } else {
        // It's a Jarray type. Using it's parent's field info: java.lang.Object.
        structType = static_cast<MIRJarrayType*>(pointedType)->GetParentType();
      }
      CHECK_FATAL(structType != nullptr, "null ptr check");
      fromType = structType->GetFieldType(irnode->GetFieldID());
    } else {
      fromType = GlobalTables::GetTypeTable().GetTypeFromTyIdx(ptrType->GetPointedTyIdx());
    }
  } else if (opnd->GetOpCode() == OP_regread) {
    auto *regReadNode = static_cast<RegreadNode*>(opnd);
    MIRPreg *mirPreg = currFunc->GetPregTab()->PregFromPregIdx(regReadNode->GetRegIdx());
    CHECK_FATAL(mirPreg->GetPrimType() == PTY_ref || mirPreg->GetPrimType() == PTY_ptr,
                "must be reference or ptr type for preg");
    if (opnd->GetPrimType() == PTY_ref) {
      fromType = mirPreg->GetMIRType();
    } else {
      const size_t gTypeTableSize = GlobalTables::GetTypeTable().GetTypeTable().size();
      CHECK_FATAL(gTypeTableSize > PTY_ptr, "container check");
      fromType = GlobalTables::GetTypeTable().GetTypeTable()[PTY_ptr];
    }
  }
  ASSERT((fromType->GetPrimType() == maple::PTY_ptr || fromType->GetPrimType() == maple::PTY_ref),
         "unknown fromType! check it!");
  ASSERT(GlobalTables::GetTypeTable().GetTypeFromTyIdx(callNode->GetTyIdx())->GetPrimType() == maple::PTY_ptr ||
         GlobalTables::GetTypeTable().GetTypeFromTyIdx(callNode->GetTyIdx())->GetPrimType() == maple::PTY_ref,
         "unknown fromType! check it!");
  CHECK_FATAL(!callNode->GetReturnVec().empty(), "container check");
  CallReturnPair callReturnPair = callNode->GetReturnVec()[0];
  StmtNode *assignReturnTypeNode = nullptr;
  if (!callReturnPair.second.IsReg()) {
    assignReturnTypeNode = builder->CreateStmtDassign(callReturnPair.first, callReturnPair.second.GetFieldID(), opnd);
  } else {
    PregIdx pregIdx = callReturnPair.second.GetPregIdx();
    MIRPreg *mirPreg = currFunc->GetPregTab()->PregFromPregIdx(pregIdx);
    assignReturnTypeNode = builder->CreateStmtRegassign(mirPreg->GetPrimType(), pregIdx, opnd);
  }
  currFunc->GetBody()->ReplaceStmt1WithStmt2(&stmt, assignReturnTypeNode);
}

void CheckCastGenerator::GenAllCheckCast() {
  auto &stmtNodes = currFunc->GetBody()->GetStmtNodes();
  for (auto &stmt : stmtNodes) {
    if (stmt.GetOpCode() == OP_intrinsiccallwithtypeassigned || stmt.GetOpCode() == OP_intrinsiccallwithtype) {
      auto &callNode = static_cast<IntrinsiccallNode&>(stmt);
      if (callNode.GetIntrinsic() == INTRN_JAVA_CHECK_CAST) {
        GenCheckCast(stmt);
        if (stmt.GetOpCode() == OP_intrinsiccallwithtype) {
          currFunc->GetBody()->RemoveStmt(&stmt);
        }
      }
    }
  }
}

BaseNode *CheckCastGenerator::GetObjectShadow(BaseNode *opnd) {
  FieldID fieldID = builder->GetStructFieldIDFromFieldNameParentFirst(WKTypes::Util::GetJavaLangObjectType(),
                                                                      namemangler::kShadowClassName);
  BaseNode *ireadExpr =
      builder->CreateExprIread(*GlobalTables::GetTypeTable().GetPtr(), *pointerObjType, fieldID, opnd);
  return ireadExpr;
}


void CheckCastGenerator::ProcessFunc(MIRFunction *func) {
  if (func->IsEmpty()) {
    return;
  }
  SetCurrentFunction(*func);
  GenAllCheckCast();
  MIRLower mirlowerer(GetMIRModule(), func);
  mirlowerer.LowerFunc(*func);
}
}  // namespace maple
