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
 *- @Brief: 字符串常量在各种情况下出现冗余时，都应该用优化后的同一个伪寄存器地址。
 *  -#step1: 校验中间文件StmtTest14.VtableImpl.mpl中 callassigned &MCC_GetOrInsertLiteral出现的次数：1次。
 *- @Expect:AB\n
 *- @Priority: High
 *- @Source: StmtTest14.java
 *- @ExecuteClass: StmtTest14
 *- @ExecuteArgs:
 */

import java.nio.charset.StandardCharsets;

public class StmtTest14 {
    public static void main(String[] args) {
        String string = "AB"; // 1
        for (int ii = 0; ii < 10; ii++) {
            string += "A" + "B"; // +号拼接场景
        }
        if (string.length() == 22) {
            string = "AB"; // 作为入参
        } else {
            char[] chars = "AB".toCharArray(); // 作为函数调用主体
            string = chars.toString();
        }
        byte[] bs = string.getBytes();
        string = new String(bs, StandardCharsets.US_ASCII);
        System.out.println(string);
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan AB\n