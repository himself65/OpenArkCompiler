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
 * -@TestCaseID: RTFieldSet3
 *- @RequirementName: Java Reflection
 *- @TestCaseName:RTFieldSet3.java
 *- @Title/Destination: Setting on a final field throws IllegalAccessException.
 *- @Brief:no:
 * -#step1: 定义类FieldSet3_a，包含final修饰的变量。定义类FieldSet3_a的子类FieldSet3，包含final修饰的变量。
 * -#step2: 通过调用forName()方法加载类FieldSet3，调用newInstance()方法生成实例对象。
 * -#step3: 通过调用 getField()获取对应名称的成员变量，调用set()方法设置变量的值，抛出IllegalAccessException。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: RTFieldSet3.java
 *- @ExecuteClass: RTFieldSet3
 *- @ExecuteArgs:
 */

import java.lang.reflect.Field;

class FieldSet3_a {
    public final static String str = "aaa";
    public int num = 2;
}

class FieldSet3 extends FieldSet3_a {
    public final int num = 1;
}

public class RTFieldSet3 {
    public static void main(String[] args) {
        try {
            Class cls = Class.forName("FieldSet3");
            Object obj = cls.newInstance();
            Field q1 = cls.getField("str");
            Field q2 = cls.getField("num");
            q1.set(obj, "bbb");
            q2.set(obj, 10);
        } catch (ClassNotFoundException e1) {
            System.err.println(e1);
            System.out.println(2);
        } catch (InstantiationException e2) {
            System.err.println(e2);
            System.out.println(2);
        } catch (NoSuchFieldException e3) {
            System.err.println(e3);
            System.out.println(2);
        } catch (IllegalAccessException e4) {
            System.out.println(0);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n