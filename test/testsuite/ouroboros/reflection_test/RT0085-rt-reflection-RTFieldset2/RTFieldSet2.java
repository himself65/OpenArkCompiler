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
 * -@TestCaseID: RTFieldSet2
 *- @RequirementName: Java Reflection
 *- @TestCaseName:RTFieldSet2.java
 *- @Title/Destination: If this Field object is inaccessible or final, throws IllegalAccessException; if the field
 *                      doesn't exist in the class object, throws IllegalArgumentException; trying to set a non-static
 *                      field on a null object throws NullPointerException.
 *- @Brief:no:
 * -#step1: 定义类FieldSet2_a的子类FieldSet2，含有私有变量。
 * -#step2: 通过调用forName()方法加载类FieldSet2，调用newInstance()方法生成实例对象，通过调用 getField()获取对应名称的
 *          成员变量，通过set()方法设值，抛出IllegalAccessException。
 * -#step3: 定义类FieldSet2_a，通过调用forName()方法加载类FieldSet2和FieldSet2_b，调用newInstance()方法生成实例对象，
 *          通过调用 getField()获取对应名称的成员变量，通过set()方法设值，抛出IllegalArgumentException。
 * -#step4: 通过调用forName()方法加载类FieldSet2，调用getDeclaredField()方法获取对应名称的成员变量，通过set()方法设值，
 *          抛出NullPointerException。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: RTFieldSet2.java
 *- @ExecuteClass: RTFieldSet2
 *- @ExecuteArgs:
 */

import java.lang.reflect.Field;

class FieldSet2_a {
    public static String str = "aaa";
    public int num;
}

class FieldSet2 extends FieldSet2_a {
    public static String str;
    private int num = 1;
    public boolean aBoolean = true;
    public int num1 = 8;
}

class FieldSet2_b {
    public int num = 18;
}

public class RTFieldSet2 {
    public static void main(String[] args) {
        try {
            Class cls = Class.forName("FieldSet2");
            Object obj = cls.newInstance();
            Field field = cls.getDeclaredField("num");
            field.set(obj, 10);
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
            try {
                Class cls = Class.forName("FieldSet2");
                Class cls1 = Class.forName("FieldSet2_b");
                Object instance1 = cls1.newInstance();
                Field f1 = cls.getDeclaredField("aBoolean");
                f1.set(instance1, 10);
            } catch (ClassNotFoundException e5) {
                System.err.println(e5);
                System.out.println(2);
            } catch (InstantiationException e6) {
                System.err.println(e6);
                System.out.println(2);
            } catch (NoSuchFieldException e7) {
                System.err.println(e7);
                System.out.println(2);
            } catch (IllegalAccessException e8) {
                System.err.println(e8);
                System.out.println(2);
            } catch (IllegalArgumentException e9) {
                try {
                    Class cls = Class.forName("FieldSet2");
                    Field f2 = cls.getDeclaredField("aBoolean");
                    f2.set(null, false);
                } catch (ClassNotFoundException e10) {
                    System.err.println(e10);
                    System.out.println(2);
                } catch (NoSuchFieldException e11) {
                    System.err.println(e11);
                    System.out.println(2);
                } catch (IllegalAccessException e12) {
                    System.err.println(e12);
                    System.out.println(2);
                } catch (NullPointerException e13) {
                    System.out.println(0);
                }
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n