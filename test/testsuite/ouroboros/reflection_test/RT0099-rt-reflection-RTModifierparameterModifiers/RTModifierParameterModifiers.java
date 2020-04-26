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
 * -@TestCaseID: RTModifierParameterModifiers
 *- @RequirementName: Java Reflection
 *- @TestCaseName:RTModifierParameterModifiers.java
 *- @Title/Destination: Verify that the integer value of the parameter modifier for the modifier class is 16.
 *                      Modifier.parameterModifiers() return an int value OR-ing together the source language modifiers
 *                      that can be applied to a parameter.
 *- @Brief:no:
 * -#step1: 经判断得知Modifier.parameterModifiers()的返回值为16，即修饰符类的参数修饰符的整数值为16；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: RTModifierParameterModifiers.java
 *- @ExecuteClass: RTModifierParameterModifiers
 *- @ExecuteArgs:
 */

import java.lang.reflect.Modifier;

public class RTModifierParameterModifiers {
    public static void main(String[] args) {
        if (Modifier.parameterModifiers() == 16) {
            System.out.println(0);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n