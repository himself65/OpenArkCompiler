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
 * -@TestCaseID: ReflectPermissionExPermissioncheckGuard.java
 * -@TestCaseName: Exception in reflect ReflectPermission:  void checkGuard(Object object)
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create a ReflectPermission object
 * -#step2: call method checkGuard(Object object) of new Object
 * -#step3: Confirm that execute is correct, no exception is thrown
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ReflectPermissionExPermissioncheckGuard.java
 * -@ExecuteClass: ReflectPermissionExPermissioncheckGuard
 * -@ExecuteArgs:
 */

import java.lang.reflect.ReflectPermission;

public class ReflectPermissionExPermissioncheckGuard {
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
            result = reflectPermissionExPermissioncheckGuard1();
        } catch (Exception e) {
            ReflectPermissionExPermissioncheckGuard.res = ReflectPermissionExPermissioncheckGuard.res - 20;
        }

        if (result == 4 && ReflectPermissionExPermissioncheckGuard.res == 89) {
            result = 0;
        }

        return result;
    }

    private static int reflectPermissionExPermissioncheckGuard1() {
        int result1 = 4; /*STATUS_FAILED*/
        // void checkGuard(Object object)
        String name = "name";
        String actions = null;
        final Object obj = new Object();
        ReflectPermission rp = new ReflectPermission(name, actions);
        try {
            rp.checkGuard(obj);
            ReflectPermissionExPermissioncheckGuard.res = ReflectPermissionExPermissioncheckGuard.res - 10;
        } catch (IllegalMonitorStateException e) {
            ReflectPermissionExPermissioncheckGuard.res = ReflectPermissionExPermissioncheckGuard.res - 1;
        }

        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n