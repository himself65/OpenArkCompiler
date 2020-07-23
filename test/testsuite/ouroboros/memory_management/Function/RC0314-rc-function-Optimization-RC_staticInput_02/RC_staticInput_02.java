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
 * -@TestCaseID:maple/runtime/rc/function/RC_staticInput_02.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Brief:functionTest
 *- @Expect:ExpectResult\nExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: RC_staticInput_02.java
 *- @ExecuteClass: RC_staticInput_02
 *- @ExecuteArgs:
 */
public class RC_staticInput_02 {
    static int[] test = {10, 20, 30, 40};

    public static void main(String[] args) {
        if (test.length == 4)
            System.out.println("ExpectResult");
        else
            System.out.println("ErrorResult");
        test(4, test);
        if (test.length == 4)
            System.out.println("ExpectResult");
        else
            System.out.println("ErrorResult");
    }

    public static void test(int first, int[] second) {
        int[] xyz = {23, 24, 25, 26};
        modify(xyz);
        if (second.length == 4)
            System.out.println("ExpectResult");
        else
            System.out.println("ErrorResult");
    }

    public static void modify(int[] temp) {
        test = temp;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\nExpectResult\nExpectResult\n
