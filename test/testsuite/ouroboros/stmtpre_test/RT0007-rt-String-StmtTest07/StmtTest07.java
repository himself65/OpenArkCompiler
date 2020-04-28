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
 * -@TestCaseID: Maple_CompilerOptimization_StmtTest07
 *- @TestCaseName: StmtTest07
 *- @TestCaseType: Function Testing
 *- @RequirementName: Store PRE和Stmt PRE使能(store PRE 收益十分有限，先挂起)
 *- @Brief: 在同一个函数统一分支(try{For循环嵌套}catch{})中出现的字符串常量冗余时，只需要为该字符串常量调用一次runtime的MCC_GetOrInsertLiteral函数。
 *  -#step1: 在test()函数用for多层嵌套、try-多catch创造for嵌套和互斥分支，并冗余使用字符串常量
 *  -#step2: 校验中间文件SmtmTest07.VtableImpl.mpl中 callassigned &MCC_GetOrInsertLiteral出现的次数：4次。
 *- @Expect:AA\nExpectResult\n
 *- @Priority: High
 *- @Source: StmtTest07.java
 *- @ExecuteClass: StmtTest07
 *- @ExecuteArgs:
 */

public class StmtTest07 {
    public static void main(String[] args) {
        if (test()) {
            System.out.println("ExpectResult");  // 1
        } else {
            String string = "ExpectResult" + "ExpectResult"; // 2
            System.out.println(string);
        }
    }

    // 基础的测试for循环嵌套
    private static boolean test() {
        String string = "AA"; // 3
        for (int ii = 0; ii < 10; ii++) {
            string = "AA";
            for (int jj = 0; jj < 10; jj++) {
                for (int kk = 0; kk < 2; kk++) {
                    string += "AA";
                }
            }
        }
        try {
            string = "AA";
            for (int jj = 0; jj < 10; jj++) {
                for (int ii = 0; ii < getInt(); ii++) {
                    string += "123";  // 4
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
// ASSERT: scan AA\nExpectResult\n