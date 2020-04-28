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
 * -@TestCaseID: Maple_CompilerOptimization_StmtTest05
 *- @TestCaseName: StmtTest05
 *- @TestCaseType: Function Testing
 *- @RequirementName: Store PRE和Stmt PRE使能(store PRE 收益十分有限，先挂起)
 *- @Brief:在同一个函数中，有Switch-多case分支时，在同一个数据流中出现的冗余字符串常量，只需要为该字符串常量调用一次runtime的MCC_GetOrInsertLiteral函数。
 *  -#step1: 在main行数用switch - case创造两个互斥分支。
 *  -#step2: 校验中间文件StmtTest05.VtableImpl.mpl中 callassigned &MCC_GetOrInsertLiteral出现的次数：6次。
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: StmtTest05.java
 *- @ExecuteClass: StmtTest05
 *- @ExecuteArgs:
 */

public class StmtTest05 {
    public static void main(String[] args) {
        String test = "TestStringForStmt"; // 1
        switch (test) {
            case "TestStringForStmt33": // 6
                test = "TestStringForStmt01"; // 2
                break;
            case "TestStringForStmt55":  // 3
                test += "TestStringForStmt01"; // 4
                break;
            default:
                test = "TestStringForStmt";
                break;
        }

        String output = "ExpectResult"; // 5
        if (test == "TestStringForStmt") {
            System.out.println(output);
        } else {
            System.out.print("ExpectResult");
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n