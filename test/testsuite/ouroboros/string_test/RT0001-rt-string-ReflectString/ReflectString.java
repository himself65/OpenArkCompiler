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
 * -@TestCaseID: ReflectString.java
 * -@TestCaseName: String.class.newInstance
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1:create String instance by Reflect
 * -#step1:check the instance corret
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ReflectString.java
 * -@ExecuteClass: ReflectString
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class ReflectString {
    static int res = 99;

    public static void main(String argv[]) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String argv[], PrintStream out) {
        int result = 2/*STATUS_FAILED*/;

        try {
            ReflectString_1();
        } catch (Exception e) {
            res -= 10;
        }
        if (result == 2 && res == 95) {
            result = 0;
        }
        return result;

    }

    public static void ReflectString_1() {
        int result1 = 4; /*STATUS_FAILED*/
        Object test1 = null;
        try {
            test1 = String.class.newInstance();
            res -= 4;
        } catch (InstantiationException e1) {
            System.err.println(e1);
        } catch (IllegalAccessException e2) {
            System.err.println(e2);
        }
//        System.out.println("test1:"+ test1);

    }

}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n