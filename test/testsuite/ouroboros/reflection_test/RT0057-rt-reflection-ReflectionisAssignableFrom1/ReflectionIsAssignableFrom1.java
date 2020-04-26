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
 * -@TestCaseID: ReflectionIsAssignableFrom1
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionIsAssignableFrom1.java
 *- @Title/Destination: Class.isAssignableFrom(cls) return true when Class is superclass of cls.
 *- @Brief:no:
 * -#step1: 通过Class.forName()方法分别获取AssignableFrom1类、AssignableFrom1_a类、AssignableFromTest1类和
 *          AssignableFromTest1_a类的运行时类并记为clazz1、clazz2、clazz3、clazz4；
 * -#step2: 调用isAssignableFrom确定step1中成功获取到clazz1、clazz2、clazz3、clazz4并且clazz1是clazz2的父类，而clazz2不是
 *          clazz1的父类；同理，clazz3是clazz4的父类，clazz4不是clazz3的父类；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionIsAssignableFrom1.java
 *- @ExecuteClass: ReflectionIsAssignableFrom1
 *- @ExecuteArgs:
 */

class AssignableFrom1 {
}

class AssignableFrom1_a extends AssignableFrom1 {
}

interface AssignableFromTest1 {
}

interface AssignableFromTest1_a extends AssignableFromTest1 {
}

public class ReflectionIsAssignableFrom1 {
    public static void main(String[] args) {
        try {
            Class clazz1 = Class.forName("AssignableFrom1");
            Class clazz2 = Class.forName("AssignableFrom1_a");
            Class clazz3 = Class.forName("AssignableFromTest1");
            Class clazz4 = Class.forName("AssignableFromTest1_a");
            if (clazz1.isAssignableFrom(clazz2) && !clazz2.isAssignableFrom(clazz1) && clazz3.isAssignableFrom(clazz4)
                    && !clazz4.isAssignableFrom(clazz3)) {
                System.out.println(0);
            }
        } catch (ClassNotFoundException e) {
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n