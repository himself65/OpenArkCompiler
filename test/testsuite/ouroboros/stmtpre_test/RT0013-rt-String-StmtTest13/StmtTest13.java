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
 * -@TestCaseID: Maple_CompilerOptimization_StmtTest13
 *- @TestCaseName: StmtTest13
 *- @TestCaseType: Function Testing
 *- @RequirementName: Store PRE和Stmt PRE使能(store PRE 收益十分有限，先挂起)
 *- @Brief: 字符串常量在字符串常量池中，引用变量拼接和new出来的在堆上，不会做该优化，实例变量上的字符串常量不参与常量。
 *  -#step1: 校验中间文件StmtTest13.VtableImpl.mpl中 callassigned &MCC_GetOrInsertLiteral出现的次数：3次。
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: StmtTest13.java
 *- @ExecuteClass: StmtTest13
 *- @ExecuteArgs:
 */

public class StmtTest13 {
    private String outOfMethod = "AB"; //3

    public static void main(String[] args) {
        String string = "AB"; // 1
        for (int ii = 0; ii < 10; ii++) {
            string += "A" + "B";
        }
        if (string.length() == 22) {
            string = "AB"; //此处在堆上分配内存，不会用第22行的伪寄存器地址
        } else {
            char[] chars = {'A', 'B'};
            string = chars.toString();
        }
        if (string.equals(new StmtTest13().outOfMethod)) {
            System.out.println("ExpectResult");  // 2
        } else {
            System.out.print("ExpectResult");  //优化外提，这边不会再取一次。
        }
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n