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
 * -@TestCaseID: ThreadGroupExObjectnotifyAllIllegalMonitorStateException.java
 * -@TestCaseName: Exception in ThreadGroup:  final void notifyAll()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1:Prepare current thread is not the owner of the object's monitor
 * -#step2:Test api notifyAll extends from Object
 * -#step3:Throw IllegalMonitorStateException
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ThreadGroupExObjectnotifyAllIllegalMonitorStateException.java
 * -@ExecuteClass: ThreadGroupExObjectnotifyAllIllegalMonitorStateException
 * -@ExecuteArgs:
 */

import java.lang.Thread;

public class ThreadGroupExObjectnotifyAllIllegalMonitorStateException {
    static int res = 99;
    ThreadGroup gr1 = new ThreadGroup("Thread8023");

    public static void main(String argv[]) {
        System.out.println(new ThreadGroupExObjectnotifyAllIllegalMonitorStateException().run());
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = threadGroupExObjectnotifyAllIllegalMonitorStateException1();
        } catch (Exception e) {
            ThreadGroupExObjectnotifyAllIllegalMonitorStateException.res = ThreadGroupExObjectnotifyAllIllegalMonitorStateException.res - 20;
        }

        Thread t1 = new Thread(new ThreadGroupExObjectnotifyAllIllegalMonitorStateException11(1));
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

        if (result == 4 && ThreadGroupExObjectnotifyAllIllegalMonitorStateException.res == 58) {
            result = 0;
        }

        return result;
    }

    private int threadGroupExObjectnotifyAllIllegalMonitorStateException1() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        //  final void notifyAll()
        try {
            gr1.notifyAll();
            ThreadGroupExObjectnotifyAllIllegalMonitorStateException.res = ThreadGroupExObjectnotifyAllIllegalMonitorStateException.res - 10;
        } catch (IllegalMonitorStateException e2) {
            ThreadGroupExObjectnotifyAllIllegalMonitorStateException.res = ThreadGroupExObjectnotifyAllIllegalMonitorStateException.res - 1;
        }

        return result1;
    }

    private class ThreadGroupExObjectnotifyAllIllegalMonitorStateException11 implements Runnable {
        //  final void notifyAll()
        private int remainder;

        private ThreadGroupExObjectnotifyAllIllegalMonitorStateException11(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (gr1) {
                try {
                    gr1.notifyAll();
                    ThreadGroupExObjectnotifyAllIllegalMonitorStateException.res = ThreadGroupExObjectnotifyAllIllegalMonitorStateException.res - 40;
                } catch (IllegalMonitorStateException e2) {
                    ThreadGroupExObjectnotifyAllIllegalMonitorStateException.res = ThreadGroupExObjectnotifyAllIllegalMonitorStateException.res - 30;
                }
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n