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
 * -@TestCaseID: ReflectionForName1
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionForName1.java
 *- @Title/Destination: Use Class.forName() To find a class by class name
 *- @Brief:no:
 * -#step1: 通过Class.forName的方法获取ForName1类的类型clazz；
 * -#step2: 确定step1中的clazz与ForName1类是同一类型；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionForName1.java
 *- @ExecuteClass: ReflectionForName1
 *- @ExecuteArgs:
 */

class ForName1 {
}

public class ReflectionForName1 {
    public static void main(String[] args) throws ClassNotFoundException {
        Class clazz = Class.forName("ForName1");
        if (clazz.toString().equals("class ForName1")) {
            System.out.println(0);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n