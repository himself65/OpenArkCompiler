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
 * -@TestCaseID: ReflectionAsSubclass2
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionAsSubclass2.java
 *- @Title/Destination: To cast a subclass of another class by reflection get a class
 *- @Brief:no:
 * -#step1: 通过Class.forName("B_1")，再对其返回值调用asSubclass()方法可以获得B类的子类B_1类；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionAsSubclass2.java
 *- @ExecuteClass: ReflectionAsSubclass2
 *- @ExecuteArgs:
 */

class B {
}

class B_1 extends B {
}

public class ReflectionAsSubclass2 {
    public static void main(String[] args) {
        try {
            Class.forName("B_1").asSubclass(B.class);
        } catch (ClassCastException e) {
            System.out.println(2);
        } catch (ClassNotFoundException e) {
            System.out.println(2);
        }
        System.out.println(0);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n