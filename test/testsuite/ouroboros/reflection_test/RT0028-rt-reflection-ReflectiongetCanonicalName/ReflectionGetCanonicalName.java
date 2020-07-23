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
 * -@TestCaseID: ReflectionGetCanonicalName
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionGetCanonicalName.java
 *- @Title/Destination: Class.getCanonicalName() returns class name by reflection.
 *- @Brief:no:
 * -#step1: Define an empty class.
 * -#step2: Get Class instance by calling ForName(String className)
 * -#step3: Test getCanonicalName() returns class name correctly.
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionGetCanonicalName.java
 *- @ExecuteClass: ReflectionGetCanonicalName
 *- @ExecuteArgs:
 */

class GetCanonicalNameTest {
}

public class ReflectionGetCanonicalName {

    public static void main(String[] args) {
        int result = 0; /* STATUS_Success */

        try {
            Class zqp = Class.forName("GetCanonicalNameTest");
            if (zqp.getCanonicalName().equals("GetCanonicalNameTest")) {
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