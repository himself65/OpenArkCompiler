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
 * -@TestCaseID: ReflectionIsSynthetic
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionIsSynthetic.java
 *- @Title/Destination: Returns true if this class is a synthetic class
 *- @Brief:no:
 * -#step1: Define a test class IsSynthetic.
 * -#step2: Test isSynthetic() with three different type object.
 * -#step3: Check that isSynthetic() identifies synthetic class correctly.
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionIsSynthetic.java
 *- @ExecuteClass: ReflectionIsSynthetic
 *- @ExecuteArgs:
 */

public class ReflectionIsSynthetic {
    public static void main(String[] args) {
        int result = 0; /* STATUS_Success */
        new IsSynthetic();
        try {
            Class zqp1 = IsSynthetic.class;
            Class zqp2 = int.class;
            Class zqp3 = Class.forName("ReflectionIsSynthetic$1");
            if (!zqp2.isSynthetic()) {
                if (!zqp1.isSynthetic()) {
                    if (zqp3.isSynthetic()) {
                        result = 0;
                    }
                }
            }
        } catch (ClassNotFoundException e) {
            result = -1;
        }
        System.out.println(result);
    }

    private static class IsSynthetic {
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n