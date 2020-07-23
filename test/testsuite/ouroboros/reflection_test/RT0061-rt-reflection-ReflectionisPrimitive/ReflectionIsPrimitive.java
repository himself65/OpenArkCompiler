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
 * -@TestCaseID: ReflectionIsPrimitive
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionIsPrimitive.java
 *- @Title/Destination: Determines if the specified Class object represents a primitive type.
 *- @Brief:no:
 * -#step1: Get nine Class Pre-defined object and a normal Class object of class.
 * -#step2: Test isPrimitive() with the 10 primitive type object.
 * -#step3: Check that isPrimitive() identifies primitive type of the object correctly.
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionIsPrimitive.java
 *- @ExecuteClass: ReflectionIsPrimitive
 *- @ExecuteArgs:
 */

public class ReflectionIsPrimitive {
    public static void main(String[] args) {
        int result = 0; /* STATUS_Success */

        Class zqp1 = ReflectionIsPrimitive.class;
        Class zqp2 = int.class;
        Class zqp3 = boolean.class;
        Class zqp4 = byte.class;
        Class zqp5 = char.class;
        Class zqp6 = short.class;
        Class zqp7 = long.class;
        Class zqp8 = float.class;
        Class zqp9 = double.class;
        Class zqp10 = void.class;
        if (!zqp1.isPrimitive() && zqp2.isPrimitive() && zqp3.isPrimitive() && zqp4.isPrimitive() && zqp5.isPrimitive()
                && zqp6.isPrimitive() && zqp7.isPrimitive() && zqp8.isPrimitive() && zqp9.isPrimitive() &&
                zqp10.isPrimitive()) {
            result = 0;
        }
        System.out.println(result);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n