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
 * -@TestCaseID: RTFieldSet1
 *- @RequirementName: Java Reflection
 *- @TestCaseName:RTFieldSet1.java
 *- @Title/Destination: Field.set() sets the field on the specified object argument to the new value.
 *- @Brief:no:
 * -#step1: 定义类FieldSet1_a，定义类FieldSet1_a的子类FieldSet1。
 * -#step2: 通过调用forName()方法加载类FieldSet1，调用newInstance()方法生成实例对象。
 * -#step3: 通过调用getField()获取对应名称的成员变量，调用set()方法设置变量的值。
 * -#step4: 通过调用get()方法获取对应字段的值并作比较，确认字段值正确。
 * -#step5: 通过调用forName()方法加载类FieldSet1_a，调用newInstance()方法生成实例对象。
 * -#step6: 通过调用getDeclaredField()获取对应名称的成员变量，调用set()方法设置变量的值。
 * -#step7: 通过调用get()方法获取对应字段的值并作比较，确认字段值正确。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: RTFieldSet1.java
 *- @ExecuteClass: RTFieldSet1
 *- @ExecuteArgs:
 */

import java.lang.reflect.Field;

class FieldSet1_a {
    public static String str = "aaa";
    public int num2 = 2;
    public int number = 5;
    public static int num1;
}

class FieldSet1 extends FieldSet1_a {
    public static String str = "bbb";
    public int num = 1;
    public int test = super.number + 1;
    public static boolean aBoolean = true;
    public char aChar;
}

public class RTFieldSet1 {
    public static void main(String[] args) {
        try {
            Class cls = Class.forName("FieldSet1");
            Object obj = cls.newInstance();
            Field f1 = cls.getField("str");
            Field f2 = cls.getField("num");
            Field f3 = cls.getField("aBoolean");
            Field f4 = cls.getField("aChar");
            Field f5 = cls.getField("num1");
            Field f6 = cls.getField("num2");
            Field f7 = cls.getField("number");
            f1.set(obj, "ccc");
            f2.set(obj, 10);
            f3.set(obj, false);
            f4.set(obj, '国');
            f5.set(obj, 20);
            f6.set(obj, 30);
            f7.set(obj, 40);
            if (f1.get(obj).toString().equals("ccc") && (int) f2.get(obj) == 10 && !(boolean) f3.get(obj) && (int)
                    f6.get(obj) == 30) {
                if (f4.get(obj).toString().equals("国") && (int) f5.get(obj) == 20) {
                    if ((int) cls.getField("test").get(obj) == 6) {
                        Class cls1 = Class.forName("FieldSet1_a");
                        Object instance1 = cls1.newInstance();
                        Object p = cls1.getDeclaredField("str").get(instance1);
                        if (p.toString().equals("aaa")) {
                            System.out.println(0);
                        }
                    }
                }
            }
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
            System.err.println(e4);
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n