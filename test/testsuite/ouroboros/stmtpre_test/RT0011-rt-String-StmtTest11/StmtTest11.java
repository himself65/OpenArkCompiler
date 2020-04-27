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
 * -@TestCaseID: Maple_CompilerOptimization_StmtTest11
 *- @TestCaseName: StmtTest11
 *- @TestCaseType: Function Testing
 *- @RequirementName: Store PRE和Stmt PRE使能(store PRE 收益十分有限，先挂起)
 *- @Brief: 在同一个函数同一数据流分支(For/If-else/try-catch/switch-case)中出现的字符串常量冗余时，只需要为该字符串常量调用一次runtime的MCC_GetOrInsertLiteral函数。
 *  -#step1: 在main方法中函数有try-catch/switch-case/if-else嵌套使用，并冗余使用字符串常量
 *  -#step2: 校验中间文件StmtTest11.VtableImpl.mpl中 callassigned &MCC_GetOrInsertLiteral出现的次数：6次。
 *- @Expect:AB\nExpectResult\n
 *- @Priority: High
 *- @Source: StmtTest11.java
 *- @ExecuteClass: StmtTest11
 *- @ExecuteArgs:
 */

public class StmtTest11 {
    public static void main(String[] args) {
        if (test()) {
            System.out.println("ExpectResult");  // 1
        } else {
            String string = "ExpectResultExpectResult"; // 2
            System.out.println(string);
        }
    }

    // 基础的测试for循环嵌套
    private static boolean test() {
        String string = "AA"; // 3
        try {
            string = "AB"; // 4
            if (string.length() == 2) {
                for (int jj = 0; jj < 10; jj++) {
                    for (int ii = 0; ii < getInt(); ii++) {
                        string += "123";  // 5,被外提
                    }
                }
            } else {
                switch (string) {
                    case "A": // 6
                        string = "123"; //被外提
                        break;
                    case "AA":
                        string = "A";
                        break;
                    default:
                        break;
                }
            }
        } catch (ArithmeticException a) {
            System.out.println(string);
        }
        return string.length() == 2;  //true;
    }

    private static int getInt() {
        return 1 / 0;
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan AB\nExpectResult\n