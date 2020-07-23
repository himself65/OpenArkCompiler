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
 * -@TestCaseID: Maple_CompilerOptimization_StmtTest09
 *- @TestCaseName: StmtTest09
 *- @TestCaseType: Function Testing
 *- @RequirementName: Store PRE和Stmt PRE使能(store PRE 收益十分有限，先挂起)
 *- @Brief: 在同一个函数同一个分支(多层switch嵌套)中出现的字符串常量冗余，只需要为该字符串常量调用一次runtime的MCC_GetOrInsertLiteral函数。涉及到函数调用时，不会做该优化
 *  -#step1: 在main方法中函数有switch-多case分支，并冗余使用字符串常量
 *  -#step2: 校验中间文件StmtTest09.VtableImpl.mpl中callassigned &MCC_GetOrInsertLiteral出现的次数：8次。
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: StmtTest09.java
 *- @ExecuteClass: StmtTest09
 *- @ExecuteArgs:
 */

public class StmtTest09 {
    public static void main(String[] args) {
        String test = "TestStringForStmt"; // 1
        switch (test) {
            case "TestStringForStmt33": // 2
                test = "TestStringForStmt01"; // 3
                break;
            case "TestStringForStmt55":  // 4
                test += "TestStringForStmt01"; // 其他优化导致"TestStringForStmt01"的地址调用外提到22行，所以与33行合用。
                break;
            default:
                switch (test) {
                    case "TestStringForStmt34": // 5
                        test = "TestStringForStmt01"; // 其他优化导致"TestStringForStmt01"的地址调用外提到22行，所以与33行合用
                        break;
                    case "TestStringForStmt35":  // 6
                        test += "TestStringForStmt01"; // 7
                        break;
                    default:
                        test = "TestStringForStmt";
                        break;
                }
        }
        String output = "ExpectResult"; // 8、
        if (test == "TestStringForStmt") {
            System.out.println(output);
        } else {
            System.out.print("ExpectResult");
        }

    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n