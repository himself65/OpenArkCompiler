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
 * -@TestCaseID: ReflectionDesiredAssertionStatus
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionDesiredAssertionStatus.java
 *- @Title/Destination: When a class's assertionStatus is not set, desiredAssertionStatus() return false
 *- @Brief:no:
 * -#step1: 创建一个ReflectionDesiredAssertionStatus类的实例对象reflectionDesiredAssertionStatus；
 * -#step2: 通过reflectionDesiredAssertionStatus的getClass()方法获取其所属的类型并记为clazz；
 * -#step3: 调用clazz的desiredAssertionStatus()方法得到的返回值为false；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionDesiredAssertionStatus.java
 *- @ExecuteClass: ReflectionDesiredAssertionStatus
 *- @ExecuteArgs:
 */

public class ReflectionDesiredAssertionStatus {
    public static void main(String[] args) {
        ReflectionDesiredAssertionStatus reflectionDesiredAssertionStatus = new ReflectionDesiredAssertionStatus();
        Class clazz = reflectionDesiredAssertionStatus.getClass();
        if (!clazz.desiredAssertionStatus()) {
            System.out.println(0);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n