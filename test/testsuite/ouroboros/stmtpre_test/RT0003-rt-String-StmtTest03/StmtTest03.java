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
 * -@TestCaseID: Maple_CompilerOptimization_StmtTest03
 *- @TestCaseName: StmtTest03
 *- @TestCaseType: Function Testing
 *- @RequirementName: Store PRE和Stmt PRE使能(store PRE 收益十分有限，先挂起)
 *- @Brief:涉及接口实现的场景下，在同一个函数同一分支中出现的字符串常量，只需要为该字符串常量调用一次runtime的MCC_GetOrInsertLiteral函数。
 *  -#step1: 创建一个实现接口的类，在该类中实现接口中的函数test()，在该函数中反复调用一个特定的字符串常量做相关操作;
 *  -#step2: 在main行数用if-else创造两个互斥分支。
 *  -#step3: 校验中间文件SmtmTest03.VtableImpl.s中callassigned &MCC_GetOrInsertLiteral出现的次数：3次。
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: StmtTest03.java
 *- @ExecuteClass: StmtTest03
 *- @ExecuteArgs:
 */

interface Interface {
    boolean test();
}

public class StmtTest03 implements Interface {
    public static void main(String[] args) {
        if (new StmtTest03().test()) {
            System.out.println("ExpectResult");  // 1
        } else {
            System.out.println("ExpectResult " + "ExpectResult"); // 2
        }
    }

    // 实现接口
    public boolean test() {
        String str1 = "A" + "A"; // 3
        String str2 = "A" + "A";
        return str1 == str2;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n