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
 * -@TestCaseID: ReflectionToString
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionToString.java
 *- @Title/Destination: To obtain a class for string output by reflection.
 *- @Brief:no:
 * -#step1: Create a test class.
 * -#step2: Use classloader to load class.
 * -#step3: Test toString() by reflection.
 * -#step4: Check that reflection result is correct.
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionToString.java
 *- @ExecuteClass: ReflectionToString
 *- @ExecuteArgs:
 */

@interface Fff {
}

@Fff
abstract class ToString_$<t> {
}

public class ReflectionToString {
    public static void main(String[] args) {
        int result = 0; /* STATUS_Success */
        try {
            Class zqp = Class.forName("ToString_$");
            String zhu = zqp.toString();
            if (zhu.equals("class ToString_$")) {
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
// ASSERT: scan 0\n