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
 * -@TestCaseID: Maple_CompilerOptimization_StmtTest06
 *- @TestCaseName: StmtTest06
 *- @TestCaseType: Function Testing
 *- @RequirementName: Store PRE和Stmt PRE使能(store PRE 收益十分有限，先挂起)
 *- @Brief:在同一个函数中，有try-多catch分支时，在同一个数据流中出现的冗余字符串常量，只需要为该字符串常量调用一次runtime的MCC_GetOrInsertLiteral函数。
 *  -#step1: 在main行数用stry-多catch创造两个互斥分支
 *  -#step2: 校验中间文件StmtTest06.VtableImpl.mpl中 callassigned &MCC_GetOrInsertLiteral出现的次数：3次。
 *- @Expect:123#\n
 *- @Priority: High
 *- @Source: StmtTest06.java
 *- @ExecuteClass: StmtTest06
 *- @ExecuteArgs:
 */

public class StmtTest06 {
    public static void main(String[] argv) {
        int result = 2;
        boolean check;
        String str = "123#";  // 1
        try {
            Integer.parseInt(str);
        } catch (NumberFormatException e) {
            str = "123#456";  // 2
            result--;
        } catch (NullPointerException e) {
            str = "123456";  // 3
            result = 2;
        } catch (OutOfMemoryError e) {
            str += "123#456";  // 被外提
            result = 2;
        } finally {
            check = str == "123#456";
            result--;
        }
        if (check == true && result == 0) {
            System.out.println("123#");
        }
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 123#\n