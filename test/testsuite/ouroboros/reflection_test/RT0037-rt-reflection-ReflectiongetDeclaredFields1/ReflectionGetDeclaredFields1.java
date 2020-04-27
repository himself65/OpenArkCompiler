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
 * -@TestCaseID: ReflectionGetDeclaredFields1
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionGetDeclaredFields1.java
 *- @Title/Destination: Class.GetDeclaredFields() returns an array of Field objects reflecting all the fields declared
 *                      by the class or interface represented by this Class object. This includes public, protected,
 *                      default (package) access, and private fields, but excludes inherited fields.
 *- @Brief:no:
 * -#step1: Create test classes and interface.
 * -#step2: Use classloader to load class.
 * -#step3: Check that return Field objects array correctly by calling GetDeclaredFields().
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionGetDeclaredFields1.java
 *- @ExecuteClass: ReflectionGetDeclaredFields1
 *- @ExecuteArgs:
 */

import java.lang.reflect.Field;

class GetDeclaredFields1_a {
    public int i_a = 5;
    String s_a = "bbb";
}

class GetDeclaredFields1 extends GetDeclaredFields1_a {
    public int i = 1;
    String s = "aaa";
    private double d = 2.5;
    protected float f = -222;
}

interface GetDeclaredFields1_b {
    public int i_b = 2;
    String s_b = "ccc";
}

public class ReflectionGetDeclaredFields1 {
    public static void main(String[] args) {
        int result = 0; /* STATUS_Success */
        try {
            Class zqp1 = Class.forName("GetDeclaredFields1");
            Class zqp2 = Class.forName("GetDeclaredFields1_b");
            Field[] j = zqp1.getDeclaredFields();
            Field[] k = zqp2.getDeclaredFields();
            if (j.length == 4 && k.length == 2) {
                result = 0;
            }
        } catch (ClassNotFoundException e1) {
            System.err.println(e1);
            result = -1;
        }
        System.out.println(result);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n