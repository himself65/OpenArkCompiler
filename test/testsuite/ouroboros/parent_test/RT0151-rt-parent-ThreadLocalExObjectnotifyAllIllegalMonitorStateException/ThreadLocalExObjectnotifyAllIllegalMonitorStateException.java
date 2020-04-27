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
 * -@TestCaseID: ThreadLocalExObjectnotifyAllIllegalMonitorStateException.java
 * -@TestCaseName: Exception in ThreadLocal: final void notifyAll()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1:Prepare current thread is not the owner of the object's monitor
 * -#step2:Test ThreadLocal api notifyAll extends from Object
 * -#step3:Throw IllegalMonitorStateException
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ThreadLocalExObjectnotifyAllIllegalMonitorStateException.java
 * -@ExecuteClass: ThreadLocalExObjectnotifyAllIllegalMonitorStateException
 * -@ExecuteArgs:
 */

import java.lang.Thread;

public class ThreadLocalExObjectnotifyAllIllegalMonitorStateException {
    static int res = 99;
    ThreadLocal<Object> tl1 = new ThreadLocal<Object>();

    public static void main(String argv[]) {
        System.out.println(new ThreadLocalExObjectnotifyAllIllegalMonitorStateException().run());
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = threadLocalExObjectnotifyAllIllegalMonitorStateException1();
        } catch (Exception e) {
            ThreadLocalExObjectnotifyAllIllegalMonitorStateException.res = ThreadLocalExObjectnotifyAllIllegalMonitorStateException.res - 20;
        }

        Thread t1 = new Thread(new ThreadLocalExObjectnotifyAllIllegalMonitorStateException11(1));
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

        if (result == 4 && ThreadLocalExObjectnotifyAllIllegalMonitorStateException.res == 58) {
            result = 0;
        }

        return result;
    }

    private int threadLocalExObjectnotifyAllIllegalMonitorStateException1() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        //  final void notifyAll()
        try {
            tl1.notifyAll();
            ThreadLocalExObjectnotifyAllIllegalMonitorStateException.res = ThreadLocalExObjectnotifyAllIllegalMonitorStateException.res - 10;
        } catch (IllegalMonitorStateException e2) {
            ThreadLocalExObjectnotifyAllIllegalMonitorStateException.res = ThreadLocalExObjectnotifyAllIllegalMonitorStateException.res - 1;
        }

        return result1;
    }

    private class ThreadLocalExObjectnotifyAllIllegalMonitorStateException11 implements Runnable {
        //  final void notifyAll()
        private int remainder;

        private ThreadLocalExObjectnotifyAllIllegalMonitorStateException11(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (tl1) {
                try {
                    tl1.notifyAll();
                    ThreadLocalExObjectnotifyAllIllegalMonitorStateException.res = ThreadLocalExObjectnotifyAllIllegalMonitorStateException.res - 40;
                } catch (IllegalMonitorStateException e2) {
                    ThreadLocalExObjectnotifyAllIllegalMonitorStateException.res = ThreadLocalExObjectnotifyAllIllegalMonitorStateException.res - 30;
                }
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n