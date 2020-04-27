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
 * -@TestCaseID: ThreadExObjecthashCode.java
 * -@TestCaseName: Exception in Thread: int hashCode()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create Object1 by new Thread()
 * -#step2: Create Object2 is given by Object2, Object3 by new Thread()
 * -#step3: Check hashCode of the Object1 and Object2 is equal, check hashCode of the Object1 and Object3 is not equal.
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ThreadExObjecthashCode.java
 * -@ExecuteClass: ThreadExObjecthashCode
 * -@ExecuteArgs:
 */

import java.lang.Thread;

public class ThreadExObjecthashCode {
    static int res = 99;

    public static void main(String argv[]) {
        System.out.println(new ThreadExObjecthashCode().run());
    }

    /**
     * main test fun
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = threadExObjecthashCode1();
        } catch (Exception e) {
            ThreadExObjecthashCode.res = ThreadExObjecthashCode.res - 20;
        }
        if (result == 4 && ThreadExObjecthashCode.res == 89) {
            result = 0;
        }

        return result;
    }


    private int threadExObjecthashCode1() {
        int result1 = 4; /*STATUS_FAILED*/
        // int hashCode()

        Thread thr1 = new Thread();
        Thread thr2 = thr1;
        Thread thr3 = new Thread();
        thr1.setUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
            @Override
            public void uncaughtException(Thread t, Throwable e) {
                System.out.println(t.getName() + " : " + e.getMessage());
            }
        });
        thr2.setUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
            @Override
            public void uncaughtException(Thread t, Throwable e) {
                System.out.println(t.getName() + " : " + e.getMessage());
            }
        });
        thr3.setUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
            @Override
            public void uncaughtException(Thread t, Throwable e) {
                System.out.println(t.getName() + " : " + e.getMessage());
            }
        });

        if (thr1.hashCode() == thr2.hashCode() && thr1.hashCode() != thr3.hashCode()) {
            ThreadExObjecthashCode.res = ThreadExObjecthashCode.res - 10;
        } else {
            ThreadExObjecthashCode.res = ThreadExObjecthashCode.res - 5;
        }

        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n