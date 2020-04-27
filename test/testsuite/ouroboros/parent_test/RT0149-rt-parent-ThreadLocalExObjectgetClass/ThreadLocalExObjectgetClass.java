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
 * -@TestCaseID: ThreadLocalExObjectgetClass.java
 * -@TestCaseName: Exception in ThreadLocal:  final Class<?> getClass()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:Test ThreadLocal api getClass extends from Object
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ThreadLocalExObjectgetClass.java
 * -@ExecuteClass: ThreadLocalExObjectgetClass
 * -@ExecuteArgs:
 */

import java.lang.ThreadLocal;

public class ThreadLocalExObjectgetClass {
    static int res = 99;

    public static void main(String argv[]) {
        System.out.println(new ThreadLocalExObjectgetClass().run());
    }

    /**
     * main test fun
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = threadLocalExObjectgetClass1();
        } catch (Exception e) {
            ThreadLocalExObjectgetClass.res = ThreadLocalExObjectgetClass.res - 20;
        }

        if (result == 4 && ThreadLocalExObjectgetClass.res == 89) {
            result = 0;
        }

        return result;
    }

    private int threadLocalExObjectgetClass1() {
        int result1 = 4; /*STATUS_FAILED*/
        //  final Class<?> getClass()
        ThreadLocal<Object> threadLocal1 = new ThreadLocal<Object>();
        Class px1 = threadLocal1.getClass();

        if (px1.toString().equals("class java.lang.ThreadLocal")) {
            ThreadLocalExObjectgetClass.res = ThreadLocalExObjectgetClass.res - 10;
        }
        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n