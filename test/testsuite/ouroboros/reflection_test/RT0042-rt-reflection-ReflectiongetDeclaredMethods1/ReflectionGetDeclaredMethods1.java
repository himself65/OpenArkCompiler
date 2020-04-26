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
 * -@TestCaseID: ReflectionGetDeclaredMethods1
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionGetDeclaredMethods1.java
 *- @Title/Destination: Class.GetDeclaredMethods returns an array containing Method objects reflecting all the declared
 *                      methods of the class or interface represented by this Class object, including public, protected,
 *                      default (package) access, and private methods. If class has multiple declared methods with the
 *                      same name and parameter types but different return types, each method will be returned.
 *- @Brief:no:
 * -#step1: 通过Class.forName()方法获取GetDeclaredMethods1类的运行时类clazz1；
 * -#step2: 通过Class.forName()方法获取GetDeclaredMethods1_b类的运行时类clazz2；
 * -#step3: 通过getDeclaredMethods()方法，分别获取clazz1、clazz2的所有的方法对象，返回值分别为methods1和methods2；
 * -#step4: 确定step3中methods1和methods2都成功获取，即成功获取到clazz1、clazz2的所有的方法对象，并且methods1、methods2
 *          的长度分别为6和2。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionGetDeclaredMethods1.java
 *- @ExecuteClass: ReflectionGetDeclaredMethods1
 *- @ExecuteArgs:
 */

import java.lang.reflect.Method;

class GetDeclaredMethods1_a {
    public void empty1() {
    }

    public void empty2() {
    }
}

class GetDeclaredMethods1 extends GetDeclaredMethods1_a {
    public void void1() {
    }

    void void2() {
    }

    int getZero() {
        return 0;
    }

    private String getDd() {
        return "dd";
    }

    public void setNumber(int number) {
    }

    int setName(String name) {
        return 2;
    }
}

interface GetDeclaredMethods1_b {
    public default void test1() {
    }

    default void test2() {
    }
}

public class ReflectionGetDeclaredMethods1 {
    public static void main(String[] args) {
        try {
            Class clazz1 = Class.forName("GetDeclaredMethods1");
            Class clazz2 = Class.forName("GetDeclaredMethods1_b");
            Method[] methods1 = clazz1.getDeclaredMethods();
            Method[] methods2 = clazz2.getDeclaredMethods();
            if (methods1.length == 6 && methods2.length == 2) {
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