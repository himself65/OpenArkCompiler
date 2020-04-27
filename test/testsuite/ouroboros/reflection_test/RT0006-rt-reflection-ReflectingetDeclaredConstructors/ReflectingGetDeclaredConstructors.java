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
 * -@TestCaseID: ReflectingGetDeclaredConstructors
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectingGetDeclaredConstructors.java
 *- @Title/Destination: Call Class.getDeclaredConstructors() to get all the local constructors of a class by reflection
 *- @Brief:no:
 * -#step1: 通过反射的方法获得GetDeclaredConstructors类的一个实例对象getDeclaredConstructors；
 * -#step2: 通过调用getDeclaredConstructors的GetDeclaredConstructors()方法，获取到它的所有本地构造函数并记为constructors；
 * -#step3: 确定step2中获取getDeclaredConstructors对象的所有本地构造函数constructors成功，并且其长度为4；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectingGetDeclaredConstructors.java
 *- @ExecuteClass: ReflectingGetDeclaredConstructors
 *- @ExecuteArgs:
 */

import java.lang.reflect.Constructor;

class GetDeclaredConstructors {
    public GetDeclaredConstructors() {
    }

    private GetDeclaredConstructors(String name) {
    }

    protected GetDeclaredConstructors(String name, int number) {
    }

    GetDeclaredConstructors(int number) {
    }
}

public class ReflectingGetDeclaredConstructors {
    public static void main(String[] args) {
        try {
            Class getDeclaredConstructors = Class.forName("GetDeclaredConstructors");
            Constructor<?>[] constructors = getDeclaredConstructors.getDeclaredConstructors();
            if (constructors.length == 4) {
                System.out.println(0);
            }
        } catch (ClassNotFoundException e1) {
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n