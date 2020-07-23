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
 * -@TestCaseID: Maple_CompilerOptimization_StmtTest08
 *- @TestCaseName: StmtTest08
 *- @TestCaseType: Function Testing
 *- @RequirementName: Store PRE和Stmt PRE使能(store PRE 收益十分有限，先挂起)
 *- @Brief: 在同一个函数统一分支if-else多分支中出现的字符串常量冗余时，同一数据流里的冗余字符串常量只需要为该字符串常量才会调用一次runtime的MCC_GetOrInsertLiteral函数。
 *  -#step1: 在test1()函数有多层if-else分支，并冗余使用字符串常量
 *  -#step2: 校验中间文件StmtTest08.VtableImpl.mpl中callassigned &MCC_GetOrInsertLiteral出现的次数：7次。
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: StmtTest08.java
 *- @ExecuteClass: StmtTest08
 *- @ExecuteArgs:
 */

public class StmtTest08 {
    public static void main(String[] args) {
        if (test1() == false) {
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
        String str1 = "AA"; // 4
        String str2;
        if (test() == true) {
            str2 = str1;
            if (str2 == "AA") {
                str2 = str2.substring(0, 1);
            } else {
                str1 = "AAA"; // 5
            }
        } else {
            str2 = "A"; // 6
            if (str2.equals("AA")) {
                str2 = str2.substring(0, 1);
            } else {
                str1 = "AAA"; // 7
            }
        }
        return str2 == str1; //true
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n