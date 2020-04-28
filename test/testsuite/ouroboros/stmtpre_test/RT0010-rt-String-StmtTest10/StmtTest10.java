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
 * -@TestCaseID: Maple_CompilerOptimization_StmtTest10
 *- @TestCaseName: StmtTest10
 *- @TestCaseType: Function Testing
 *- @RequirementName: Store PRE和Stmt PRE使能(store PRE 收益十分有限，先挂起)
 *- @Brief: 在同一个函数同一个分支(多层try-多cathc嵌套)中出现的字符串常量冗余，只需要为该字符串常量调用一次runtime的MCC_GetOrInsertLiteral函数。涉及到函数调用时，不会做该优化
 *  -#step1: 在main方法中函数有两层try-多catch分支，并冗余使用字符串常量
 *  -#step2: 校验中间文件StmtTest10.VtableImpl.mpl中 callassigned &MCC_GetOrInsertLiteral出现的次数：4次。
 *           注："123#456"的函数调用会外提，因此在实际的中间代码里出现的次数是4次。
 *- @Expect:123#\n
 *- @Priority: High
 *- @Source: StmtTest10.java
 *- @ExecuteClass: StmtTest10
 *- @ExecuteArgs:
 */

public class StmtTest10 {
    public static void main(String[] argv) {
        int result = 2;
        boolean check;
        String str = "123#";  // 1
        try {
            str = "123456"; // 2
            try {
                Integer.parseInt(str);
            } catch (NumberFormatException e) {
                str = "123#456";  // 3
                result--;
            } catch (NullPointerException e) {
                str = "123456#"; // 4
                result = 2;
            } catch (OutOfMemoryError e) {
                str += "123#456";  // 与32行被其他优化了
                result = 2;
            } finally {
                str = "123#456";
            }
            Integer.parseInt(str);
        } catch (NumberFormatException e) {
            str = "123#456";
            result--;
        } catch (NullPointerException e) {
            str = "123456";
            result = 2;
        } catch (OutOfMemoryError e) {
            str += "123#456";
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