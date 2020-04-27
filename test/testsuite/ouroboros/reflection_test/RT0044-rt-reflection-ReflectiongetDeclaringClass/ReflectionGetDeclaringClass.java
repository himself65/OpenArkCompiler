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
 * -@TestCaseID: ReflectionGetDeclaringClass
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionGetDeclaringClass.java
 *- @Title/Destination: If the class or interface represented by this Class object is a member of another class,
 *                      Class.GetDeclaringClass() returns the Class object representing the class in which it was
 *                      declared. This method returns null if this class or interface is not a member of any other class.
 *- @Brief:no:
 * -#step1: 通过Class.forName()方法获取GetDeclaringClass类的运行时类clazz；
 * -#step2: 通过getDeclaredFields()方法获取clazz的所有的成员，返回值为一个数组记为fields；
 * -#step3: 通过getDeclaredConstructors()方法获取clazz的所有的构造方法，返回值为一个数组记为constructors；
 * -#step4: 对于num < fields.length，fields[i].getDeclaringClass().getName()的返回值与GetDeclaringClass类是同一类型，同时
 *          对于str < constructors.length，constructors[s].getDeclaringClass().getName()的返回值与GetDeclaringClass类是同
 *          一类型；
 * -#step5: 在step4的基础上，通过Class.forName()方法获取GetDeclaringClass_a类的运行时类clazz2，并且通过
 *          clazz2.getDeclaringClass()的返回值均为null；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionGetDeclaringClass.java
 *- @ExecuteClass: ReflectionGetDeclaringClass
 *- @ExecuteArgs:
 */

import java.lang.reflect.Constructor;
import java.lang.reflect.Field;

class GetDeclaringClass {
    public int num;
    String str;
    float fNum;

    public GetDeclaringClass() {
    }

    GetDeclaringClass(int number) {
    }

    GetDeclaringClass(String name) {
    }

    GetDeclaringClass(int number, String name) {
    }
}

class GetDeclaringClass_a {
}

public class ReflectionGetDeclaringClass {
    public static void main(String[] args) {
        try {
            int num = 0;
            Class clazz = Class.forName("GetDeclaringClass");
            Field[] fields = clazz.getDeclaredFields();
            Constructor<?>[] constructors = clazz.getDeclaredConstructors();
            for (int i = 0; i < fields.length; i++) {
                if (fields[i].getDeclaringClass().getName().equals("GetDeclaringClass")) {
                    for (int j = 0; j < constructors.length; j++) {
                        if (constructors[j].getDeclaringClass().getName().equals("GetDeclaringClass")) {
                            Class clazz2 = Class.forName("GetDeclaringClass_a");
                            if (clazz2.getDeclaringClass() == null) {
                                num++;
                            }
                        }
                    }
                }
            }
            if (num == 12) {
                System.out.println(0);
            }
        } catch (ClassNotFoundException e) {
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n