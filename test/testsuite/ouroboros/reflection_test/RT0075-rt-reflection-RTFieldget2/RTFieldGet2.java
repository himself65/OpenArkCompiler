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
 * -@TestCaseID: RTFieldGet2
 *- @RequirementName: Java Reflection
 *- @TestCaseName:RTFieldGet2.java
 *- @Title/Destination: Field.get throws IllegalAccessException when the Field is inaccessible; throws
 *                      IllegalArgumentException when the object is not an instance of the class; Field.get(null) throws
 *                      NullPointerException.
 *- @Brief:no:
 * -#step1: 创建类FieldGet2_a，FieldGet2_a的子类FieldGet2且含有私有成员变量，定义FieldGet2_b。
 * -#step2：通过forName()方法加载FieldGet2并调用newInstance()生成实例对象。
 * -#step3：通过getDeclaredField()方法获取FieldGet2中的私有变量，确认会抛出IllegalAccessException。
 * -#step4：加载FieldGet2实例对象，通过getDeclaredField()方法获取public变量域，构造FieldGet2_b实例，并取对应变量域值。
 * -#step5：确认会抛出IllegalArgumentException异常，通过getDeclaredField()方法获取FieldGet2的public变量域。
 * -#step6：调用get(Object obj)方法，obj为NULL，确认NullPointerException抛出。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: RTFieldGet2.java
 *- @ExecuteClass: RTFieldGet2
 *- @ExecuteArgs:
 */

class FieldGet2_a {
    public int num;
}

class FieldGet2 extends FieldGet2_a {
    private int number = 1;
    public boolean aBoolean = true;
    public int number1 = 8;
}

class FieldGet2_b {
    public int number1 = 18;
}

public class RTFieldGet2 {
    public static void main(String[] args) {
        try {
            Class cls = Class.forName("FieldGet2");
            Object obj = cls.newInstance();
            Object q1 = cls.getDeclaredField("number").get(obj);
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
                Class cls = Class.forName("FieldGet2");
                Class cls1 = Class.forName("FieldGet2_b");
                Object instance1 = cls1.newInstance();
                Object q2 = cls.getDeclaredField("number1").get(instance1);
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
                    Class cls = Class.forName("FieldGet2");
                    Object q3 = cls.getDeclaredField("aBoolean").get(null);
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
// ASSERT: scan-full 0\n