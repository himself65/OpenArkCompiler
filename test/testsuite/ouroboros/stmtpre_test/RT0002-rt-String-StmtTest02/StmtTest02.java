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
 * -@TestCaseID: Maple_CompilerOptimization_StmtTest02
 *- @TestCaseName: StmtTest02
 *- @TestCaseType: Function Testing
 *- @RequirementName: Store PRE和Stmt PRE使能(store PRE 收益十分有限，先挂起)
 *- @Brief:在同一个函数同一个分支中出现的字符串常量冗余，只需要为该字符串常量调用一次runtime的MCC_GetOrInsertLiteral函数。涉及到另外函数调用时，不会做该优化
 *  -#step1: 创建一个函数，在该函数中反复调用一个特定的字符串常量做相关操作;再起另一个函数调用该函数，在这个函数中也有该常量字符串的操作。
 *  -#step2: 在main行数用if-else创造两个互斥分支。
 *  -#step3: 校验中间文件SmtmTest02.VtableImpl.mpl中callassigned &MCC_GetOrInsertLiteral出现的次数：4次。
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: StmtTest02.java
 *- @ExecuteClass: StmtTest02
 *- @ExecuteArgs:
 */

public class StmtTest02 {
    public static void main(String[] args) {
        if (test1()) {
            System.out.println("ExpectResult"); // 1
        } else {
            System.out.println("ExpectResult " + "ExpectResult"); // 2
        }
    }

    // 基础的测试
    private static boolean test() {
        String string = "A"; // 3
        for (int ii = 0; ii < 100; ii++) {
            string += "A";
        }
        return string.length() == 101;  //true;
    }

    // 有函数调用,确认
    private static boolean test1() {
        String str1 = "A" + "A"; // 4
        String str2 = "A" + "A";
        if (test() == true) {
            str2 = str1;
        }
        return str1 == str1; //true
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n