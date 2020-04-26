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
 * -@TestCaseID: ReflectionGetMethod2
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionGetMethod2.java
 *- @Title/Destination: If expected method is not found, getMethod throws NoSuchMethodException; Class.getMethod(NULL)
 *                      throws NullPointerException.
 *- @Brief:no:
 * -#step1: 通过Class.forName()方法获取GetMethod2类的运行时类clazz1；
 * -#step2: 以empty为参数，通过getMethod()方法获取clazz1的公共成员方法并记为method1；
 * -#step3: 通过Class.forName()方法获取GetMethod2类的运行时类clazz2；
 * -#step4: 以null为参数，通过getMethod()方法获取clazz2的公共成员方法并记为method2；
 * -#step5: 确定step2和step4中的method1、method2获取失败，并且会抛出NullPointerException；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionGetMethod2.java
 *- @ExecuteClass: ReflectionGetMethod2
 *- @ExecuteArgs:
 */

import java.lang.reflect.Method;

class GetMethod2_a {
    public int getName(String name) {
        return 10;
    }

    public String getString() {
        return "dda";
    }
}

class GetMethod2 extends GetMethod2_a {
    public void getVoid() {
    }

    void empty() {
    }

    int getZero() {
        return 0;
    }

    private String getStr() {
        return "dd";
    }

    public void setNum(int number) {
    }

    int getSecondNum(String name) {
        return 2;
    }
}

public class ReflectionGetMethod2 {
    public static void main(String[] args) {
        try {
            Class clazz1 = Class.forName("GetMethod2");
            Method method1 = clazz1.getMethod("empty");
            System.out.println(2);
        } catch (ClassNotFoundException e1) {
            System.err.println(e1);
            System.out.println(2);
        } catch (NoSuchMethodException e2) {
            // NoSuchMethodException is thrown when method not exist.
            try {
                Class clazz2 = Class.forName("GetMethod2");
                Method method2 = clazz2.getMethod(null);
                System.out.println(2);
            } catch (ClassNotFoundException e4) {
                System.err.println(e4);
                System.out.println(2);
            } catch (NoSuchMethodException e5) {
                System.err.println(e5);
                System.out.println(2);
            } catch (NullPointerException e6) {
                // Expected result: NullPointerException is thrown when method is null.
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