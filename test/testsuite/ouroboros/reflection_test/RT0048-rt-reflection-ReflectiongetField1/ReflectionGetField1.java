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
 * -@TestCaseID: ReflectionGetField1
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionGetField1.java
 *- @Title/Destination: Class.getField() returns a Field object that reflects the specified public member field,
 *                      including the inherited public fields.
 *- @Brief:no:
 * -#step1: 通过Class.forName()方法获取GetField1类的运行时类clazz；
 * -#step2: 分别以num、str为参数，通过getField()方法分别获取GetField1类、GetField1_a类的公共字段并记为field1、field2；
 * -#step3: 确定step2中成功获取到两个字段field1、field2，并且field1等于i，field2等于str。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionGetField1.java
 *- @ExecuteClass: ReflectionGetField1
 *- @ExecuteArgs:
 */

import java.lang.reflect.Field;

class GetField1_a {
    int num2 = 5;
    public String str = "bbb";
}

class GetField1 extends GetField1_a {
    public int num = 1;
    String string = "aaa";
    private double dNum = 2.5;
    protected float fNum = -222;
}

public class ReflectionGetField1 {
    public static void main(String[] args) {
        try {
            Class clazz = Class.forName("GetField1");
            Field field1 = clazz.getField("num");
            Field field2 = clazz.getField("str");
            if (field1.getName().equals("num") && field2.getName().equals("str")) {
                System.out.println(0);
            }
        } catch (ClassNotFoundException e1) {
            System.err.println(e1);
            System.out.println(2);
        } catch (NoSuchFieldException e2) {
            System.err.println(e2);
            System.out.println(2);
        } catch (NullPointerException e3) {
            System.err.println(e3);
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n