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
 * -@TestCaseID: ThreadGroupExObjectequals.java
 * -@TestCaseName: Exception in ThreadGroup:  boolean equals(Object obj)
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Created ThreadGroup Object1 by new ThreadGroup(String name)
 * -#step2: Created ThreadGroup Object2 by new ThreadGroup(String name) the same as Object1
 * -#step3: call equals(Object obj), and check the result is correctly.
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ThreadGroupExObjectequals.java
 * -@ExecuteClass: ThreadGroupExObjectequals
 * -@ExecuteArgs:
 */

import java.lang.Thread;

public class ThreadGroupExObjectequals {
    static int res = 99;

    public static void main(String argv[]) {
        System.out.println(new ThreadGroupExObjectequals().run());
    }

    /**
     * main test fun
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = threadGroupExObjectequals1();
        } catch (Exception e) {
            ThreadGroupExObjectequals.res = ThreadGroupExObjectequals.res - 20;
        }


        if (result == 4 && ThreadGroupExObjectequals.res == 89) {
            result = 0;
        }

        return result;
    }


    private int threadGroupExObjectequals1() {
        int result1 = 4; /*STATUS_FAILED*/
        // boolean equals(Object obj)
        ThreadGroup gr1 = new ThreadGroup("Thread3251");
        ThreadGroup gr2 = new ThreadGroup("Thread3252");
        boolean ret = gr1.equals(gr2);
        if (!ret) {
            ThreadGroupExObjectequals.res = ThreadGroupExObjectequals.res - 10;
        }
        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n