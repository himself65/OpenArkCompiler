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
 * -@TestCaseID: ReflectPermissionExBasicPermissionimplies.java
 * -@TestCaseName: Exception in reflect ReflectPermission: boolean implies(Permission p)
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create a ReflectPermission object
 * -#step2: call method implies(Permission p) of new Object
 * -#step3: Confirm that the return value is true
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ReflectPermissionExBasicPermissionimplies.java
 * -@ExecuteClass: ReflectPermissionExBasicPermissionimplies
 * -@ExecuteArgs:
 */

import java.lang.reflect.ReflectPermission;

public class ReflectPermissionExBasicPermissionimplies {
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
            result = reflectPermissionExBasicPermissionimplies1();
        } catch (Exception e) {
            ReflectPermissionExBasicPermissionimplies.res = ReflectPermissionExBasicPermissionimplies.res - 20;
        }

        if (result == 4 && ReflectPermissionExBasicPermissionimplies.res == 89) {
            result = 0;
        }

        return result;
    }

    private static int reflectPermissionExBasicPermissionimplies1() {
        //  boolean implies(Permission p)
        int result1 = 4; /*STATUS_FAILED*/
        String name = "name";
        String actions = null;
        ReflectPermission rp = new ReflectPermission(name, actions);
        if (rp.implies(rp)) {
            ReflectPermissionExBasicPermissionimplies.res = ReflectPermissionExBasicPermissionimplies.res - 10;
        } else {
            ReflectPermissionExBasicPermissionimplies.res = ReflectPermissionExBasicPermissionimplies.res - 5;
        }

        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n