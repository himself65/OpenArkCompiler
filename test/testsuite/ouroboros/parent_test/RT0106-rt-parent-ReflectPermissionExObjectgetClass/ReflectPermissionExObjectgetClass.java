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
 * -@TestCaseID: ReflectPermissionExObjectgetClass.java
 * -@TestCaseName: Exception in reflect ReflectPermission:  final Class<?> getClass()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create a ReflectPermission object1
 * -#step2: Call getClass() on Object1
 * -#step3: Confirm that the returned Class is correct
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ReflectPermissionExObjectgetClass.java
 * -@ExecuteClass: ReflectPermissionExObjectgetClass
 * -@ExecuteArgs:
 */

import java.lang.reflect.ReflectPermission;

public class ReflectPermissionExObjectgetClass {
    static int res = 99;

    public static void main(String argv[]) {
        System.out.println(run());
    }

    /**
     * main test fun
     * @return status code
     */
    public static int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = reflectPermissionExObjectgetClass1();
        } catch (Exception e) {
            ReflectPermissionExObjectgetClass.res = ReflectPermissionExObjectgetClass.res - 20;
        }

        if (result == 4 && ReflectPermissionExObjectgetClass.res == 89) {
            result = 0;
        }

        return result;
    }

    private static int reflectPermissionExObjectgetClass1() throws ClassNotFoundException {
        //  final Class<?> getClass()
        int result1 = 4; /*STATUS_FAILED*/
        String name = "name";
        String actions = null;
        ReflectPermission rp = new ReflectPermission(name, actions);
        try {
            Class<? extends ReflectPermission> cls1 = rp.getClass();
            if (cls1.toString().equals("class java.lang.reflect.ReflectPermission")) {
                ReflectPermissionExObjectgetClass.res = ReflectPermissionExObjectgetClass.res - 10;
            }
        } catch (IllegalMonitorStateException e) {
            ReflectPermissionExObjectgetClass.res = ReflectPermissionExObjectgetClass.res - 1;
        }

        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n