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
 * -@TestCaseID: ReflectionToGenericString
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionToGenericString.java
 *- @Title/Destination: getGenericString() returns a string description of this class, including information about
 *                      modifiers and type parameters.
 *- @Brief:no:
 * -#step1: Create a abstract test class.
 * -#step2: Use classloader to load class.
 * -#step3: Test getGenericString() with Class object of the test class.
 * -#step4: Check that getGenericString() returns a string description of class information correctly.
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionToGenericString.java
 *- @ExecuteClass: ReflectionToGenericString
 *- @ExecuteArgs:
 */

@interface Eee {
}

@Eee
abstract class ToGenericString<t> {
}

public class ReflectionToGenericString {
    public static void main(String[] args) {
        int result = 0; /* STATUS_Success */
        try {
            Class zqp = Class.forName("ToGenericString");
            String zhu = zqp.toGenericString();
            if (zhu.equals("abstract class ToGenericString<t>")) {
                result = 0;
            }
        } catch (ClassNotFoundException e) {
            result = -1;
        }
        System.out.println(result);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n