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
 * -@TestCaseID: ReflectionIsMemberClass
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionIsMemberClass.java
 *- @Title/Destination: Class.IsMemberClass() returns true if and only if the underlying class is a member
 *                      class(class inside another class, but not inside a method).
 *- @Brief:no:
 * -#step1: Define a class in method and two inner class but not inside the method.
 * -#step2: Get Class object of the class.
 * -#step3: Test isMemberClass() with different the Class object of the class.
 * -#step4: Check that isMemberClass() identifies member class correctly.
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionIsMemberClass.java
 *- @ExecuteClass: ReflectionIsMemberClass
 *- @ExecuteArgs:
 */

public class ReflectionIsMemberClass {
    public static void main(String[] args) {
        int result = 0; /* STATUS_Success */
        class IsMemberClass_a {
        }
        try {
            Class zqp1 = IsMemberClass.class;
            Class zqp2 = Class.forName("ReflectionIsMemberClass");
            Class zqp3 = IsMemberClass_a.class;
            Class zqp4 = IsMemberClass_b.class;
            Class zqp5 = (new IsMemberClass_b() {
            }).getClass();
            if (!zqp2.isMemberClass()) {
                if (!zqp3.isMemberClass()) {
                    if (zqp4.isMemberClass()) {
                        if (!zqp5.isMemberClass()) {
                            if (zqp1.isMemberClass()) {
                                result = 0;
                            }
                        }
                    }
                }
            }
        } catch (ClassNotFoundException e) {
            result = -1;
        }
        System.out.println(result);
    }

    class IsMemberClass {
    }

    static class IsMemberClass_b {
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n