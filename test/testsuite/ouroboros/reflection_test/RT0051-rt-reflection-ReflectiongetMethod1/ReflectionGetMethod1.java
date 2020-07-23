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
 * -@TestCaseID: ReflectionGetMethod1
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionGetMethod1.java
 *- @Title/Destination: Class.getMethod Returns a Method object that reflects the specified public member method of the
 *                      class or interface represented by this Class object. Include inherited public methods.
 *- @Brief:no:
 * -#step1: 通过Class.forName()方法获取GetMethod1类的运行时类clazz；
 * -#step2: 分别以empty、（empty，int.class）、（setName，String.class）、getStr为参数，获取clazz的指定的公共成员方法并记为
 *          method1、method2、method3、method4；
 * -#step3: 确定step2中成功获取到method1、method2、method3、method4，并且正好是GetMethod1类的指定的公共成员方法；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionGetMethod1.java
 *- @ExecuteClass: ReflectionGetMethod1
 *- @ExecuteArgs:
 */

import java.lang.reflect.Method;

class GetMethod1_a {
    public int setName(String name) {
        return 10;
    }

    public String getStr() {
        return "getDda";
    }
}

class GetMethod1 extends GetMethod1_a {
    public void empty() {
    }

    void emptyB() {
    }

    int getNum() {
        return 0;
    }

    private String getDd() {
        return "getDd";
    }

    public void empty(int number) {
    }

    int getNum(String name) {
        return 2;
    }
}

public class ReflectionGetMethod1 {
    public static void main(String[] args) {
        try {
            Class clazz = Class.forName("GetMethod1");
            Method method1 = clazz.getMethod("empty");
            Method method2 = clazz.getMethod("empty", int.class);
            Method method3 = clazz.getMethod("setName", String.class);
            Method method4 = clazz.getMethod("getStr");
            if (method1.toString().equals("public void GetMethod1.empty()")
                    && method2.toString().equals("public void GetMethod1.empty(int)")
                    && method3.toString().equals("public int GetMethod1_a.setName(java.lang.String)")
                    && method4.toString().equals("public java.lang.String GetMethod1_a.getStr()")) {
                System.out.println(0);
            }
        } catch (ClassNotFoundException e1) {
            System.err.println(e1);
            System.out.println(2);
        } catch (NoSuchMethodException e2) {
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