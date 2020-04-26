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
 * -@TestCaseID: ReflectingGetConstructor1
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectingGetConstructor1.java
 *- @Title/Destination: Getting public constructors of different arguments from reflection by calling
 *                      Class.getConstructor()
 *- @Brief:no:
 * -#step1: 通过Class.forName获得GetConstructor1类的一个实例对象getConstructor1；
 * -#step2: 分别调用GetConstructor1类的三个构造方法，从而实现从反射中获取不同参数的公共构造函数constructor1、constructor2、
 *          constructor3；
 * -#step3: 判断三个构造方法获取成功；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectingGetConstructor1.java
 *- @ExecuteClass: ReflectingGetConstructor1
 *- @ExecuteArgs:
 */

import java.lang.reflect.Constructor;

class GetConstructor1 {
    public GetConstructor1() {
    }

    public GetConstructor1(String name) {
    }

    public GetConstructor1(String name, int number) {
    }

    GetConstructor1(int number) {
    }
}

public class ReflectingGetConstructor1 {
    public static void main(String[] args) {
        try {
            Class getConstructor1 = Class.forName("GetConstructor1");
            Constructor constructor1 = getConstructor1.getConstructor(String.class);
            Constructor constructor2 = getConstructor1.getConstructor();
            Constructor constructor3 = getConstructor1.getConstructor(String.class, int.class);
            if (constructor1.toString().equals("public GetConstructor1(java.lang.String)")
                    && constructor2.toString().equals("public GetConstructor1()")
                    && constructor3.toString().equals("public GetConstructor1(java.lang.String,int)")) {
                System.out.println(0);
            }
        } catch (ClassNotFoundException e1) {
            System.out.println(2);
        } catch (NoSuchMethodException e2) {
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n