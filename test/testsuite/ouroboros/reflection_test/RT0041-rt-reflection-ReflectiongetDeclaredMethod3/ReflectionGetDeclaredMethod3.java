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
 * -@TestCaseID: ReflectionGetDeclaredMethod3
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionGetDeclaredMethod3.java
 *- @Title/Destination: Class.getDeclaredMethod() can return abstract methods of target class
 *- @Brief:no:
 * -#step1: 通过Class.forName()方法获取GetDeclaredMethod3类的运行时类clazz；
 * -#step2: 以aa为参数，通过getDeclaredMethod()方法获取clazz的方法对象method1；同理，以bb为参数，通过getMethod()方法获取
 *          clazz的方法对象method2；
 * -#step3: 确定method1和method2获取成功，并且是GetDeclaredMethod3类的指定的运行方法；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionGetDeclaredMethod3.java
 *- @ExecuteClass: ReflectionGetDeclaredMethod3
 *- @ExecuteArgs:
 */

import java.lang.reflect.Method;

abstract class GetDeclaredMethod3 {
    abstract void empty1();
    abstract public int empty2();
}

public class ReflectionGetDeclaredMethod3 {
    public static void main(String[] args) {
        try {
            Class clazz = Class.forName("GetDeclaredMethod3");
            Method method1 = clazz.getDeclaredMethod("empty1");
            Method method2 = clazz.getMethod("empty2");
            if (method1.toString().equals("abstract void GetDeclaredMethod3.empty1()")
                    && method2.toString().equals("public abstract int GetDeclaredMethod3.empty2()")) {
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