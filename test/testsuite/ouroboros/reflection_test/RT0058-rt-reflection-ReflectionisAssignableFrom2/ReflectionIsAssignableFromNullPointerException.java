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
 * -@TestCaseID: ReflectionIsAssignableFromNullPointerException
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionIsAssignableFromNullPointerException.java
 *- @Title/Destination: Class.isAssignableFrom(null) throws NullPointerException.
 *- @Brief:no:
 * -#step1: Use classloader load class.
 * -#step2: Test Class.isAssignableFrom() whit null param.
 * -#step3: Check that NullPointerException occurs when Class.isAssignableFrom()'s is null.
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionIsAssignableFromNullPointerException.java
 *- @ExecuteClass: ReflectionIsAssignableFromNullPointerException
 *- @ExecuteArgs:
 */

class AssignableFromNullPointerException {
}

public class ReflectionIsAssignableFromNullPointerException {
    public static void main(String[] args) {
        int result = 0; /* STATUS_Success */
        try {
            Class zqp1 = Class.forName("AssignableFromNullPointerException");
            zqp1.isAssignableFrom(null);
            result = -1;
        } catch (ClassNotFoundException e1) {
            result = -1;
        } catch (NullPointerException e2) {
            result = 0;
        }
        System.out.println(result);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n