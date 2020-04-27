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
 * -@TestCaseID: ReflectionForName4
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionForName4.java
 *- @Title/Destination: An exception is reported when a class is not found with reflection through a class name
 *- @Brief:no:
 * -#step1: 以“for*”、true、ForName4_a.class.getClassLoader()为参数，通过Class.forName()获取相关类的运行时类时会
 *          抛出ClassNotFoundException；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionForName4.java
 *- @ExecuteClass: ReflectionForName4
 *- @ExecuteArgs:
 */

class ForName4 {
    static {
    }
}

class ForName4_a {
    static {
    }
}

public class ReflectionForName4 extends Thread {
    public static void main(String[] args) {
        try {
            Class clazz = Class.forName("for*", true, ForName4_a.class.getClassLoader());
        } catch (ClassNotFoundException e) {
            System.out.println(0);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n