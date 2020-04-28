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
 * -@TestCaseID: ThreadGroupAllowThreadSuspensionTest
 *- @TestCaseName: Thread_ThreadGroupAllowThreadSuspensionTest.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Test ThreadGroup.allowSuspension(), it changes the value of vmAllowSuspension.
 *- @Brief: see below
 * -#step1: 通过new ThreadGroup(String name)构造新的实例。
 * -#step2：调用allowThreadSuspension(boolean b)，参数b为true。
 * -#step3：通过反射getDeclaredField获取私有域vmAllowSuspension判断值不为true。
 * -#step4：调用allowThreadSuspension(boolean b)，参数b为false。
 * -#step5：通过反射getDeclaredField获取私有域vmAllowSuspension判断值不为false。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadGroupAllowThreadSuspensionTest.java
 *- @ExecuteClass: ThreadGroupAllowThreadSuspensionTest
 *- @ExecuteArgs:
 */

import java.lang.reflect.Field;

public class ThreadGroupAllowThreadSuspensionTest {
    public static void main(String[] args) {
        ThreadGroup tg = new ThreadGroup("group");
        tg.allowThreadSuspension(true);
        try {
            if (getValue(tg, "vmAllowSuspension").toString() != "true") {
                System.out.println(2);
                return;
            }
        } catch (Throwable e) {
            System.out.println(2);
            return;
        }

        tg.allowThreadSuspension(false);
        try {
            if (getValue(tg, "vmAllowSuspension").toString() != "false") {
                System.out.println(2);
                return;
            }
        } catch (Throwable e) {
            System.out.println(2);
            return;
        }
        System.out.println(0);
    }

    // Use reflect to get the value of private field of ThreadGroup
    public static Object getValue(Object instance, String fieldName) throws IllegalAccessException, NoSuchFieldException {
        Field field = getField(instance.getClass(), fieldName);
        field.setAccessible(true);
        return field.get(instance);
    }

    public static Field getField(Class thisClass, String fieldName) throws NoSuchFieldException {
        if (fieldName == null) {
            throw new NoSuchFieldException("Error field !");
        }
        Field field = thisClass.getDeclaredField(fieldName);
        return field;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n