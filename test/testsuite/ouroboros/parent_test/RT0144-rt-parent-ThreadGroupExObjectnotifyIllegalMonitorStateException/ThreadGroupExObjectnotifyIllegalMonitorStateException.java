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
 * -@TestCaseID: ThreadGroupExObjectnotifyIllegalMonitorStateException.java
 * -@TestCaseName: Exception in ThreadGroup:  final void notify()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1:Prepare current thread is not the owner of the object's monitor
 * -#step2:Test api notify extends from Object
 * -#step3:Throw IllegalMonitorStateException
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ThreadGroupExObjectnotifyIllegalMonitorStateException.java
 * -@ExecuteClass: ThreadGroupExObjectnotifyIllegalMonitorStateException
 * -@ExecuteArgs:
 */

import java.lang.Thread;

public class ThreadGroupExObjectnotifyIllegalMonitorStateException {
    static int res = 99;
    private ThreadGroup gr1 = new ThreadGroup("Thread8023");

    public static void main(String argv[]) {
        System.out.println(new ThreadGroupExObjectnotifyIllegalMonitorStateException().run());
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = threadGroupExObjectnotifyIllegalMonitorStateException1();
        } catch (Exception e) {
            ThreadGroupExObjectnotifyIllegalMonitorStateException.res = ThreadGroupExObjectnotifyIllegalMonitorStateException.res - 20;
        }

        Thread t1 = new Thread(new ThreadGroupExObjectnotifyIllegalMonitorStateException11(1));
        t1.setUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
            @Override
            public void uncaughtException(Thread t, Throwable e) {
                System.out.println(t.getName() + " : " + e.getMessage());
            }
        });
        t1.start();
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        if (result == 4 && ThreadGroupExObjectnotifyIllegalMonitorStateException.res == 58) {
            result = 0;
        }

        return result;
    }

    private int threadGroupExObjectnotifyIllegalMonitorStateException1() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        // final void notify()
        try {
            gr1.notify();
            ThreadGroupExObjectnotifyIllegalMonitorStateException.res = ThreadGroupExObjectnotifyIllegalMonitorStateException.res - 10;
        } catch (IllegalMonitorStateException e2) {
            ThreadGroupExObjectnotifyIllegalMonitorStateException.res = ThreadGroupExObjectnotifyIllegalMonitorStateException.res - 1;
        }

        return result1;
    }

    private class ThreadGroupExObjectnotifyIllegalMonitorStateException11 implements Runnable {
        // final void notify()
        private int remainder;

        private ThreadGroupExObjectnotifyIllegalMonitorStateException11(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (gr1) {
                try {
                    gr1.notify();
                    ThreadGroupExObjectnotifyIllegalMonitorStateException.res = ThreadGroupExObjectnotifyIllegalMonitorStateException.res - 40;
                } catch (IllegalMonitorStateException e2) {
                    ThreadGroupExObjectnotifyIllegalMonitorStateException.res = ThreadGroupExObjectnotifyIllegalMonitorStateException.res - 30;
                }
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n