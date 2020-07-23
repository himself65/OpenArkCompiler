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
 * -@TestCaseID: ReflectingGetConstructors
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectingGetConstructors.java
 *- @Title/Destination: Call Class.GetConstructors() to get all the public constructors in a class by reflection
 *- @Brief:no:
 * -#step1: 通过反射的方法获得GetConstructors_a类的一个实例对象getConstructors_a；
 * -#step2: 通过调用getConstructors_a的getConstructors()方法，获取到它的所有公共构造函数并记为constructors；
 * -#step3: 确定step2中成功获取到getConstructors_a对象的所有公共构造函数constructors，并且其长度等于3，并且对于
 *          constructors[i].toString()（此处i < constructors.length = 3）不包含字符串"GetConstructors_a(int)"；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectingGetConstructors.java
 *- @ExecuteClass: ReflectingGetConstructors
 *- @ExecuteArgs:
 */

import java.lang.reflect.Constructor;

class GetConstructors {
    public GetConstructors() {
    }

    public GetConstructors(String name) {
    }

    public GetConstructors(String name, int number) {
    }

    GetConstructors(int number) {
    }

    GetConstructors(double id) {
    }

    public GetConstructors(double id, String name) {
    }
}

class GetConstructors_a extends GetConstructors {
    public GetConstructors_a() {
    }

    public GetConstructors_a(String name) {
    }

    public GetConstructors_a(String name, int number) {
    }

    GetConstructors_a(int number) {
    }
}

public class ReflectingGetConstructors {
    public static void main(String[] args) {
        try {
            Class getConstructors_a = Class.forName("GetConstructors_a");
            Constructor<?>[] constructors = getConstructors_a.getConstructors();
            if (constructors.length == 3) {
                for (int i = 0; i < constructors.length; i++) {
                    if (constructors[i].toString().indexOf("GetConstructors_a(int)") != -1) {
                        System.out.println(2);
                    }
                }
                System.out.println(0);
            }
        } catch (ClassNotFoundException e) {
            System.err.println(e);
            System.out.println(2);
        } catch (ArrayIndexOutOfBoundsException e1) {
            System.err.println(e1);
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n