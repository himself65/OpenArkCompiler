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
 * -@TestCaseID: ReflectionGetDeclaredMethod2
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionGetDeclaredMethod2.java
 *- @Title/Destination: Class.getDeclaredMethod() throws NoSuchMethodException when expected method not found in class;
 *                      Class.getDeclaredMethod(null) throws NullPointerException.
 *- @Brief:no:
 * -#step1: 通过Class.forName()方法获取GetDeclaredMethod2类的运行时类clazz1；
 * -#step2: 以empty1为参数，尝试通过getDeclaredMethod()方法尝试获取clazz1的方法对象并记为method1；
 * -#step3: 通过Class.forName()方法获取GetDeclaredMethod2类的运行时类clazz2；
 * -#step4: 以null为参数，尝试通过getDeclaredMethod()方法尝试获取clazz2的方法对象并记为method2；
 * -#step5: method1和method2获取失败，并且会抛出NullPointerException；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionGetDeclaredMethod2.java
 *- @ExecuteClass: ReflectionGetDeclaredMethod2
 *- @ExecuteArgs:
 */

import java.lang.reflect.Method;

class GetDeclaredMethod2_a {
    public void empty1() {
    }

    public void empty2() {
    }
}

class GetDeclaredMethod2 extends GetDeclaredMethod2_a {
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

public class ReflectionGetDeclaredMethod2 {
    public static void main(String[] args) {
        try {
            Class clazz1 = Class.forName("GetDeclaredMethod2");
            Method method1 = clazz1.getDeclaredMethod("empty1");
            System.out.println(2);
        } catch (ClassNotFoundException e1) {
            System.err.println(e1);
            System.out.println(2);
        } catch (NoSuchMethodException e2) {
            try {
                Class clazz2 = Class.forName("GetDeclaredMethod2");
                Method method2 = clazz2.getDeclaredMethod(null);
                System.out.println(2);
            } catch (ClassNotFoundException e4) {
                System.err.println(e4);
                System.out.println(2);
            } catch (NoSuchMethodException e5) {
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
// ASSERT: scan-full 0\n