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
 * -@TestCaseID: ThreadcountStackFramesTest.java
 *- @Title/Destination: Thread api: countStackFrames()
 *- @Brief:
 *- @Expect:expected.txt
 *- @Priority: High
 *- @Source: ThreadcountStackFramesTest.java
 *- @ExecuteClass: ThreadcountStackFramesTest
 *- @ExecuteArgs:
 */

public class ThreadcountStackFramesTest {
    static int RES = 99;
    public static void main(String[] args) {
        System.out.println(run());
    }

    private static int run() {
        int result;
        result = countStackFramesTest();
        if (result == 4 && ThreadcountStackFramesTest.RES == 89) {
            result = 0;
        }
        return result;
    }

    private static int countStackFramesTest() {
        Runnable ra = new MyThread1();
        String str = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ4294967295";
        Thread t = new Thread(ra, str);
        @SuppressWarnings("deprecation")
        int i = t.countStackFrames();
        if (i == 0) {
            ThreadcountStackFramesTest.RES -= 10;
        }
        return 4;
    }
}

class MyThread1 implements Runnable {
    private int ticket = 10;
    @SuppressWarnings("unused")
    private String name;

    public void run() {
            for (int i = 0; i < 500; i++) {
                    if (this.ticket > 0) {
                            System.out.println(Thread.currentThread().getName() + "---->" + (this.ticket--));
                    }
            }
    }

    public Object handler() {
            Thread thread = new Thread(new MyThread1());
            thread.setUncaughtExceptionHandler(new MyExceptionHandler());
            thread.start();
            return thread.getUncaughtExceptionHandler();
    }

    class MyExceptionHandler implements Thread.UncaughtExceptionHandler {

            public void uncaughtException(Thread t, Throwable e) {
                    System.out.printf("An exception has been captured\n");
                    System.out.printf("Thread:%s\n", t.getName());
                    System.out.printf("Exception: %s: %s:\n", e.getClass().getName(), e.getMessage());
                    System.out.printf("Stack Trace:\n");
                    e.printStackTrace();
                    System.out.printf("Thread status:%s\n", t.getState());
            }
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0