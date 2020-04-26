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
 * -@TestCaseID: ReflectionGetDeclaredMethod1
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionGetDeclaredMethod1.java
 *- @Title/Destination: Class.getDeclaredMethod() Returns a Method object that reflects the specified declared method
 *                      of the class or interface represented by this Class object.
 *- @Brief:no:
 * -#step1: 通过Class.forName()方法获得GetDeclaredMethod1类的运行时类clazz；
 * -#step2: 分别以（"empty1", int.class）、"getDd"、"empty1"、"empty2"、"getZero"和（"getZero", String.class）为参数，通过
 *          clazz的getDeclaredMethod()方法，获取Method类的不同的方法对象method1、method2、method3、method4、method5、method6；
 * -#step3: 确定step2中的方法对象method1、method2、method3、method4、method5、method6获取成功，并且分别是
 *          GetDeclaredMethod1类的指定的声明方法。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionGetDeclaredMethod1.java
 *- @ExecuteClass: ReflectionGetDeclaredMethod1
 *- @ExecuteArgs:
 */

import java.lang.reflect.Method;

class GetDeclaredMethod1 {
    public void empty1() {
    }

    void empty2() {
    }

    int getZero() {
        return 0;
    }

    private String getDd() {
        return "dd";
    }

    public void empty1(int number) {
    }

    int getZero(String name) {
        return 2;
    }
}

public class ReflectionGetDeclaredMethod1 {
    public static void main(String[] args) {
        try {
            Class clazz = Class.forName("GetDeclaredMethod1");
            Method method1 = clazz.getDeclaredMethod("empty1", int.class);
            Method method2 = clazz.getDeclaredMethod("getDd");
            Method method3 = clazz.getDeclaredMethod("empty1");
            Method method4 = clazz.getDeclaredMethod("empty2");
            Method method5 = clazz.getDeclaredMethod("getZero");
            Method method6 = clazz.getDeclaredMethod("getZero", String.class);
            if (method1.toString().equals("public void GetDeclaredMethod1.empty1(int)")
                    && method2.toString().equals("private java.lang.String GetDeclaredMethod1.getDd()")
                    && method3.toString().equals("public void GetDeclaredMethod1.empty1()")
                    && method4.toString().equals("void GetDeclaredMethod1.empty2()")
                    && method5.toString().equals("int GetDeclaredMethod1.getZero()")
                    && method6.toString().equals("int GetDeclaredMethod1.getZero(java.lang.String)")) {
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
// ASSERT: scan 0\n