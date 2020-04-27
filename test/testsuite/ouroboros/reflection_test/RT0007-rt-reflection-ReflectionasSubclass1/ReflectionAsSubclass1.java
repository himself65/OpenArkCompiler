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
 * -@TestCaseID: ReflectionAsSubclass1
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionAsSubclass1.java
 *- @Title/Destination: child class calls Class.asSubclass() on father class/interface. Get the instance of the subclass
 *                      that casts the class to the target class by reflection gets the target
 *- @Brief:no:
 * -#step1: 通过反射获取A_1类的运行时类，并对其返回值以A.class为参数调用asSubclass()方法，获得a_2；同理，通过反射获取
 *          A_2类的运行时类，并对其返回值以A.class为参数调用asSubclass()方法，获得a_1；
 * -#step2: 经判断得知a_2.newInstance()是A的一个实例，a_1.newInstance()也是A的一个实例；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionAsSubclass1.java
 *- @ExecuteClass: ReflectionAsSubclass1
 *- @ExecuteArgs:
 */

interface A {
}

class A_1 implements A {
}

class A_2 extends A_1 {
}

public class ReflectionAsSubclass1 {
    public static void main(String[] args) {
        try {
            Class a_2 = Class.forName("A_1").asSubclass(A.class);
            Class a_1 = Class.forName("A_2").asSubclass(A.class);
            if (a_2.newInstance() instanceof A) {
                if (a_1.newInstance() instanceof A) {
                    System.out.println(0);
                }
            }
        } catch (ClassNotFoundException e1) {
            System.out.println(e1);
        } catch (InstantiationException e2) {
            System.out.println(e2);
        } catch (IllegalAccessException e3) {
            System.out.println(e3);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n