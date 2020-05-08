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
DefType UnitType = Primary, And, Or;
DefType BypassType = Accumulator, Store, AluShift;

// Architecture name
Class ArchitectureName <string>;
// Parallelism number
Class Parallelism  <int>;

Def ArchitectureName  {cortex_a55};
Def Parallelism  {2};

// class parameters can be set as default.
// default parameters can only be placed at the end
// Class Unit :Name <UnitType, componentUnits>
Class Unit :string <UnitType, Unit[]>;
// Class Reservation :Name <Latency, dependUnits>
Class Reservation : string <int, Unit[]>;
// AnonClass Bypass : BypassNum, fromTypeReservation, toTypeReservation, BypassType
Class Bypass <int, Reservation[], Reservation[], BypassType>;

Def Unit : kUnitIdSlot0 {Primary};
Def Unit : kUnitIdSlot1 {Primary};
Def Unit : kUnitIdAgen {Primary};
Def Unit : kUnitIdHazard {Primary};
Def Unit : kUnitIdCrypto {Primary};
Def Unit : kUnitIdMul {Primary};
Def Unit : kUnitIdDiv {Primary};
Def Unit : kUnitIdBranch {Primary};
Def Unit : kUnitIdStAgu {Primary};
Def Unit : kUnitIdLdAgu {Primary};
Def Unit : kUnitIdFpAluLo {Primary};
Def Unit : kUnitIdFpAluHi {Primary};
Def Unit : kUnitIdFpMulLo {Primary};
Def Unit : kUnitIdFpMulHi {Primary};
Def Unit : kUnitIdFpDivLo {Primary};
Def Unit : kUnitIdFpDivHi {Primary};

Def Unit : kUnitIdSlotS {Or, [kUnitIdSlot0, kUnitIdSlot1]};
Def Unit : kUnitIdFpAluS {Or, [kUnitIdFpAluLo, kUnitIdFpAluHi]};
Def Unit : kUnitIdFpMulS {Or, [kUnitIdFpMulLo, kUnitIdFpMulHi]};
Def Unit : kUnitIdFpDivS {Or, [kUnitIdFpDivLo, kUnitIdFpDivHi]};

Def Unit : kUnitIdSlotD {And, [kUnitIdSlot0, kUnitIdSlot1]};
Def Unit : kUnitIdFpAluD {And, [kUnitIdFpAluLo, kUnitIdFpAluHi]};
Def Unit : kUnitIdFpMulD {And, [kUnitIdFpMulLo, kUnitIdFpMulHi]};
Def Unit : kUnitIdFpDivD {And, [kUnitIdFpDivLo, kUnitIdFpDivHi]};
Def Unit : kUnitIdSlotSHazard {And, [kUnitIdSlotS, kUnitIdHazard]};
Def Unit : kUnitIdSlotSMul {And, [kUnitIdSlotS, kUnitIdMul]};
Def Unit : kUnitIdSlotSBranch {And, [kUnitIdSlotS, kUnitIdBranch]};
Def Unit : kUnitIdSlotSAgen {And, [kUnitIdSlotS, kUnitIdAgen]};
Def Unit : kUnitIdSlotDAgen {And, [kUnitIdSlot0, kUnitIdSlot1, kUnitIdAgen]};
Def Unit : kUnitIdSlot0LdAgu {And, [kUnitIdSlot0, kUnitIdLdAgu]};
Def Unit : kUnitIdSlot0StAgu {And, [kUnitIdSlot0, kUnitIdStAgu]};
Def Unit : nothing {};

Def Reservation : kLtUndef {0};
Def Reservation : kLtShift {2, [kUnitIdSlotS]};
Def Reservation : kLtShiftReg {2, [ kUnitIdSlotS, kUnitIdHazard]};
Def Reservation : kLtAlu {3, [kUnitIdSlotS]};
Def Reservation : kLtAluShift {3, [kUnitIdSlotS]};
Def Reservation : kLtAluShiftReg {3, [kUnitIdSlotS, kUnitIdHazard]};
Def Reservation : kLtAluExtr {3, [kUnitIdSlot1]};
Def Reservation : kLtMul {4, [kUnitIdSlotS, kUnitIdMul]};
Def Reservation : kLtDiv {4, [kUnitIdSlot0, kUnitIdDiv, kUnitIdDiv]};
Def Reservation : kLtLoad1 {4, [kUnitIdSlotSAgen, kUnitIdLdAgu]};
Def Reservation : kLtStore1 {2, [kUnitIdSlotSAgen, kUnitIdStAgu]};
Def Reservation : kLtLoad2 {4, [kUnitIdSlotDAgen, kUnitIdSlot0LdAgu, kUnitIdLdAgu]};
Def Reservation : kLtStore2 {2, [ kUnitIdSlotSAgen, kUnitIdStAgu]};
Def Reservation : kLtLoad3plus {6, [kUnitIdSlotDAgen, kUnitIdSlot0LdAgu, kUnitIdLdAgu]};
Def Reservation : kLtStore3plus {2, [kUnitIdSlotDAgen, kUnitIdSlot0StAgu, kUnitIdStAgu]};
Def Reservation : kLtBranch {0, [kUnitIdSlotSBranch]};
Def Reservation : kLtFpalu {4, [kUnitIdSlotS, kUnitIdFpAluS]};
Def Reservation : kLtFconst {2, [kUnitIdSlotS, kUnitIdFpAluS]};
Def Reservation : kLtFpmul {4, [kUnitIdSlotS, kUnitIdFpMulS]};
Def Reservation : kLtFpmac {8, [kUnitIdSlotS, kUnitIdFpMulS, nothing, nothing, nothing, kUnitIdFpAluS]};
Def Reservation : kLtR2f {2, [kUnitIdSlotS, kUnitIdFpAluS]};
Def Reservation : kLtF2r {4, [kUnitIdSlotS, kUnitIdFpAluS]};
Def Reservation : kLtR2fCvt {4, [kUnitIdSlotS, kUnitIdFpAluS]};
Def Reservation : kLtF2rCvt {5, [kUnitIdSlotS, kUnitIdFpAluS]};
Def Reservation : kLtFFlags {5, [kUnitIdSlotS]};
Def Reservation : kLtFLoad64 {3, [kUnitIdSlotSAgen, kUnitIdLdAgu]};
Def Reservation : kLtFLoadMany {4, [kUnitIdSlotDAgen, kUnitIdSlot0LdAgu, kUnitIdLdAgu]};
Def Reservation : kLtFStore64 {0, [kUnitIdSlotSAgen, kUnitIdStAgu]};
Def Reservation : kLtFStoreMany {0, [kUnitIdSlotSAgen, kUnitIdSlot0StAgu, kUnitIdStAgu]};
Def Reservation : kLtAdvsimdAlu {4, [kUnitIdSlotS, kUnitIdFpAluS]};
Def Reservation : kLtAdvsimdAluQ {4, [kUnitIdSlot0, kUnitIdFpAluD]};
Def Reservation : kLtAdvsimdMul {4, [kUnitIdSlotS, kUnitIdFpMulS]};
Def Reservation : kLtAdvsimdMulQ {4, [kUnitIdSlot0, kUnitIdFpMulD]};
Def Reservation : kLtAdvsimdDivS {14, [kUnitIdSlot0, kUnitIdFpMulS, kUnitIdFpDivS]};
Def Reservation : kLtAdvsimdDivD {29, [kUnitIdSlot0, kUnitIdFpMulS, kUnitIdFpDivS]};
Def Reservation : kLtAdvsimdDivSQ {14, [kUnitIdSlotD, kUnitIdFpMulD, kUnitIdFpDivD]};
Def Reservation : kLtAdvsimdDivdQ {29, [kUnitIdSlotD, kUnitIdFpMulD, kUnitIdFpDivD]};
Def Reservation : kLtCryptoAese {3, [kUnitIdSlot0]};
Def Reservation : kLtCryptoAesmc {3, [kUnitIdSlotS]};
Def Reservation : kLtClinit {14, [kUnitIdSlotS, nothing, kUnitIdSlotDAgen, kUnitIdSlot0LdAgu,
                                     kUnitIdLdAgu, nothing, kUnitIdSlotDAgen, kUnitIdSlot0LdAgu,
                                     kUnitIdLdAgu, nothing, kUnitIdSlotDAgen, kUnitIdSlot0LdAgu,
                                     kUnitIdLdAgu]};
Def Reservation : kLtAdrpLdr {6, [kUnitIdSlotS, nothing, kUnitIdSlotDAgen, kUnitIdSlot0LdAgu,
                                     kUnitIdLdAgu]};
Def Reservation : kLtClinitTail {8, [kUnitIdSlotDAgen, kUnitIdSlot0LdAgu, kUnitIdLdAgu, nothing,
                                        kUnitIdSlotDAgen, kUnitIdSlot0LdAgu, kUnitIdLdAgu]};

Def Bypass  {0, [kLtShift, kLtShiftReg], [kLtAlu]};
Def Bypass  {1, [kLtShift], [kLtShift, kLtShiftReg, kLtAluShift, kLtAluShiftReg]};
Def Bypass  {1, [kLtShiftReg], [kLtShift, kLtShiftReg, kLtAluShift, kLtAluShiftReg]};
Def Bypass  {1, [kLtAlu, kLtAluShift, kLtAluShiftReg], [kLtAlu]};
Def Bypass  {1, [kLtAlu, kLtAluShift, kLtAluShiftReg, kLtAluExtr], [kLtAluShift], AluShift};
Def Bypass  {1, [kLtAlu, kLtAluShift, kLtAluShiftReg, kLtAluExtr], [kLtAluShiftReg], AluShift};
Def Bypass  {2, [kLtAlu, kLtAluShift, kLtAluShiftReg, kLtAluExtr], [kLtAluShift]};
Def Bypass  {2, [kLtAlu, kLtAluShift, kLtAluShiftReg, kLtAluExtr], [kLtAluShiftReg]};
Def Bypass  {2, [kLtAlu, kLtAluShift, kLtAluShiftReg, kLtAluExtr], [kLtAluExtr]};
Def Bypass  {2, [kLtAlu, kLtAluShift, kLtAluShiftReg, kLtAluExtr], [kLtShift]};
Def Bypass  {2, [kLtAlu, kLtAluShift, kLtAluShiftReg, kLtAluExtr], [kLtShiftReg]};
Def Bypass  {2, [kLtMul], [kLtMul], Accumulator};
Def Bypass  {2, [kLtMul], [kLtAlu]};
Def Bypass  {3, [kLtMul], [kLtAluShift, kLtAluShiftReg, kLtAluExtr, kLtShift, kLtShiftReg]};
Def Bypass  {2, [kLtLoad1], [kLtAlu]};
Def Bypass  {3, [kLtLoad1], [kLtAluShift, kLtAluShiftReg, kLtAluExtr, kLtShift, kLtShiftReg]};
Def Bypass  {3, [kLtLoad2], [kLtAlu]};
Def Bypass  {0, [kLtAlu], [kLtStore1, kLtStore2, kLtStore3plus], Store};
Def Bypass  {0, [kLtAluShift], [kLtStore1, kLtStore2, kLtStore3plus], Store};
Def Bypass  {0, [kLtAluShiftReg], [kLtStore1, kLtStore2, kLtStore3plus], Store};
Def Bypass  {0, [kLtAluExtr], [ kLtStore1, kLtStore2, kLtStore3plus], Store};
Def Bypass  {0, [kLtShift], [kLtStore1, kLtStore2, kLtStore3plus], Store};
Def Bypass  {0, [kLtShiftReg], [kLtStore1, kLtStore2, kLtStore3plus], Store};
Def Bypass  {1, [kLtMul], [kLtStore1, kLtStore2, kLtStore3plus], Store};
Def Bypass  {1, [kLtLoad1], [kLtStore1, kLtStore2, kLtStore3plus], Store};
Def Bypass  {1, [kLtLoad2], [kLtStore1, kLtStore2, kLtStore3plus], Store};
Def Bypass  {1, [kLtLoad3plus], [kLtStore1, kLtStore2, kLtStore3plus], Store};
Def Bypass  {0, [kLtAlu, kLtAluShift, kLtAluShiftReg, kLtAluExtr, kLtShift, kLtShiftReg], [kLtR2f]};
Def Bypass  {1, [kLtMul, kLtLoad1, kLtLoad2], [kLtR2f]};
Def Bypass  {2, [kLtAlu, kLtAluShift, kLtAluShiftReg, kLtAluExtr], [kLtR2fCvt]};
Def Bypass  {3, [kLtMul, kLtLoad1, kLtLoad2], [kLtR2fCvt]};
Def Bypass  {0, [kLtAlu, kLtAluShift, kLtAluShiftReg, kLtAluExtr, kLtShift, kLtShiftReg], [kLtBranch]};
Def Bypass  {1, [kLtFpalu, kLtFpmul, kLtR2f, kLtR2fCvt, kLtFconst], [kLtFpmac], Accumulator};
Def Bypass  {1, [kLtFLoad64, kLtFLoadMany], [kLtFpmac]};
Def Bypass  {4, [kLtFpmac], [kLtFpmac], Accumulator};
Def Bypass  {0, [kLtCryptoAese], [kLtCryptoAesmc]};
Def Bypass  {1, [kLtShiftReg], [kLtClinit]};
Def Bypass  {2, [kLtAlu, kLtAluShift, kLtAluExtr], [kLtClinit]};
Def Bypass  {3, [kLtMul, kLtLoad1], [kLtClinit]};
Def Bypass  {13, [kLtAlu], [kLtClinit]};
Def Bypass  {11, [kLtClinit], [kLtStore1, kLtStore3plus], Store};
Def Bypass  {11, [kLtClinit], [kLtR2f]};
Def Bypass  {13, [kLtClinit], [kLtR2fCvt]};
Def Bypass  {1, [kLtShiftReg], [kLtAdrpLdr]};
Def Bypass  {2, [kLtAlu, kLtAluShift, kLtAluExtr], [kLtAdrpLdr]};
Def Bypass  {3, [kLtMul, kLtLoad1], [kLtAdrpLdr]};
Def Bypass  {5, [kLtAdrpLdr], [kLtAlu]};
Def Bypass  {3, [kLtAdrpLdr], [kLtStore1, kLtStore3plus], Store};
Def Bypass  {3, [kLtAdrpLdr], [kLtR2f]};
Def Bypass  {5, [kLtAdrpLdr], [kLtR2fCvt]};
Def Bypass  {7, [kLtClinitTail], [kLtAlu]};
Def Bypass  {5, [kLtClinitTail], [kLtStore1, kLtStore3plus], Store};
Def Bypass  {5, [kLtClinitTail], [kLtR2f]};
Def Bypass  {7, [kLtClinitTail], [kLtR2fCvt]};
