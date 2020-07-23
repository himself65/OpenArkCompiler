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
 * -@TestCaseID: ReflectionIsArray
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionIsArray.java
 *- @Title/Destination: Determines if this Class object represents an array class.
 *- @Brief:no:
 * -#step1: 通过Class.forName()方法获取IsArray类的运行时类clazz；
 * -#step2: 通过getDeclaredFields()方法获取clazz的所有的声明的字段并记为fields；
 * -#step3: 通过fields.getClass()获取当前的运行时类并记为clazz1；
 * -#step4: 确定step1和step3成功获取clazz和clazz1，并且clazz1是一个数组，而clazz不是一个数组。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionIsArray.java
 *- @ExecuteClass: ReflectionIsArray
 *- @ExecuteArgs:
 */

import java.lang.reflect.Field;

class IsArray {
    public int num;
    String str;
    float fNum;
}

public class ReflectionIsArray {
    public static void main(String[] args) {
        try {
            Class clazz = Class.forName("IsArray");
            Field[] fields = clazz.getDeclaredFields();
            Class clazz1 = fields.getClass();
            if (clazz1.isArray()) {
                if (!clazz.isArray()) {
                    System.out.println(0);
                }
            }
        } catch (ClassNotFoundException e) {
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n