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
 * -@TestCaseID: ReflectionGetFields
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionGetFields.java
 *- @Title/Destination: Class.GetFields() returns an array containing Field objects reflecting all the accessible public
 *                      fields of the class or interface represented by this Class object. Including inherited public
 *                      fields.
 *- @Brief:no:
 * -#step1: 通过Class.forName()方法分别获取GetFields类、GetFields_b类、GetFields_c类的运行时类clazz1、clazz2、clazz3；
 * -#step2: 通过getFields()方法分别获取clazz1、clazz2、clazz3的所有可访问的公共字段并记为fields1、fields2、fields3；
 * -#step3: 确定step2中fields1、fields2、fields3获取成功，并且fields1和fields2的长度为2，fields的长度为0。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionGetFields.java
 *- @ExecuteClass: ReflectionGetFields
 *- @ExecuteArgs:
 */

import java.lang.reflect.Field;

class GetFields_a {
    public int num = 5;
    String str = "bbb";
}

class GetFields extends GetFields_a {
    public int num = 1;
    String str = "aaa";
    private double dNum = 2.5;
    protected float fNum = -222;
}

interface GetFields_b {
    public int num = 2;
    String str = "ccc";
}

class GetFields_c {
}

public class ReflectionGetFields {
    public static void main(String[] args) {
        try {
            Class clazz1 = Class.forName("GetFields");
            Class clazz2 = Class.forName("GetFields_b");
            Class clazz3 = Class.forName("GetFields_c");
            Field[] fields1 = clazz1.getFields();
            Field[] fields2 = clazz2.getFields();
            Field[] fields3 = clazz3.getFields();
            if (fields1.length == 2 && fields2.length == 2 && fields3.length == 0) {
                System.out.println(0);
            }
        } catch (ClassNotFoundException e1) {
            System.err.println(e1);
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n