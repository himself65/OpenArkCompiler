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
 * -@TestCaseID: ReflectionGetField2
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionGetField2.java
 *- @Title/Destination: If expected Field not found, throws NoSuchFieldException; Class.getField(null) throws
 *                      NullPointerException.
 *- @Brief:no:
 * -#step1: 通过Class.forName()方法获取GetField2类的运行时类clazz1；
 * -#step2: 以s为参数，通过getField()方法尝试获取GetField2类的default类型的字段并记为field1；
 * -#step3: 通过Class.forName()方法获取GetField2类的运行时类clazz2；
 * -#step4: 以null为参数，通过getField()方法尝试获取GetField2类的字段并记为field2；
 * -#step5: 确定step2和step4中field1、field2获取失败，并且会抛出NullPointerException。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionGetField2.java
 *- @ExecuteClass: ReflectionGetField2
 *- @ExecuteArgs:
 */

import java.lang.reflect.Field;

class GetField2_a {
    int num = 5;
    public String str = "bbb";
}

class GetField2 extends GetField2_a {
    public int num = 1;
    String str2 = "aaa";
    private double dNum = 2.5;
    protected float fNum = -222;
}

public class ReflectionGetField2 {
    public static void main(String[] args) {
        try {
            Class clazz1 = Class.forName("GetField2");
            Field field1 = clazz1.getField("str2");
            System.out.println(2);
        } catch (ClassNotFoundException e1) {
            System.err.println(e1);
            System.out.println(2);
        } catch (NoSuchFieldException e2) {
            try {
                Class clazz2 = Class.forName("GetField2");
                Field field2 = clazz2.getField(null);
                System.out.println(2);
            } catch (ClassNotFoundException e4) {
                System.err.println(e4);
                System.out.println(2);
            } catch (NoSuchFieldException e5) {
                System.err.println(e5);
                System.out.println(2);
            } catch (NullPointerException e6) {
                System.out.println(0);
            }
        } catch (NullPointerException e3) {
            System.err.println(e3);
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n