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
 * -@TestCaseID: RTFieldSet4
 *- @RequirementName: Java Reflection
 *- @TestCaseName:RTFieldSet4.java
 *- @Title/Destination: To set a value with no corresponding object on a static field by reflection.
 *- @Brief:no:
 * -#step1: 定义含私有变量的类FieldSet4。
 * -#step2: 通过调用forName()方法加载类FieldSet4，调用newInstance()方法生成实例对象。
 * -#step3: 通过调用getDeclaredField()获取对应名称的成员变量，调用set()方法设置变量的值。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: RTFieldSet4.java
 *- @ExecuteClass: RTFieldSet4
 *- @ExecuteArgs:
 */

import java.lang.reflect.Field;

class FieldSet4 {
    public static String str;
    private int num = 1;
    public boolean aBoolean = true;
    public int num1 = 8;
}

public class RTFieldSet4 {
    public static void main(String[] args) {
        try {
            Class cls = Class.forName("FieldSet4");
            Object obj = cls.newInstance();
            Field field = cls.getDeclaredField("str");
            field.set(null, "aaa");
            System.out.println(0);
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
// ASSERT: scan-full 0\n