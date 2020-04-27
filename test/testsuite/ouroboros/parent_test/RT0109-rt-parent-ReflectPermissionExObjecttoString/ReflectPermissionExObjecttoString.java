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
 * -@TestCaseID: ReflectPermissionExObjecttoString.java
 * -@TestCaseName: Exception in reflect ReflectPermission:  String toString()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create a ReflectPermission object
 * -#step2: call method toString() of new Object
 * -#step3: Confirm that the return value is correct
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ReflectPermissionExObjecttoString.java
 * -@ExecuteClass: ReflectPermissionExObjecttoString
 * -@ExecuteArgs:
 */

import java.lang.reflect.ReflectPermission;

public class ReflectPermissionExObjecttoString {
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
            result = reflectPermissionExObjecttoString1();
        } catch (Exception e) {
            ReflectPermissionExObjecttoString.res = ReflectPermissionExObjecttoString.res - 20;
        }

        if (result == 4 && ReflectPermissionExObjecttoString.res == 89) {
            result = 0;
        }

        return result;
    }

    private static int reflectPermissionExObjecttoString1() {
        int result1 = 4; /*STATUS_FAILED*/
        // String toString()
        String name = "name";
        String actions = null;
        ReflectPermission rp = new ReflectPermission(name, actions);
        try {
            String str1 = rp.toString();

            if (str1.contains("java.lang.reflect.ReflectPermission")) {
                ReflectPermissionExObjecttoString.res = ReflectPermissionExObjecttoString.res - 10;
            }
        } catch (IllegalMonitorStateException e) {
            ReflectPermissionExObjecttoString.res = ReflectPermissionExObjecttoString.res - 1;
        }

        return result1;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n