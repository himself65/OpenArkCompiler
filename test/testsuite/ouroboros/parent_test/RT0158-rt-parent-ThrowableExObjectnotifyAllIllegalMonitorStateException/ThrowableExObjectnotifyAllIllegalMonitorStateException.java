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
 * -@TestCaseID: ThrowableExObjectnotifyAllIllegalMonitorStateException.java
 * -@TestCaseName: Exception in Throwable: final void notifyAll()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1:Prepare current thread is not the owner of the object's monitor
 * -#step2:Test Throwable api notifyAll extends from Object
 * -#step3:Throw IllegalMonitorStateException
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ThrowableExObjectnotifyAllIllegalMonitorStateException.java
 * -@ExecuteClass: ThrowableExObjectnotifyAllIllegalMonitorStateException
 * -@ExecuteArgs:
 */

import java.lang.Thread;

public class ThrowableExObjectnotifyAllIllegalMonitorStateException {
    static int res = 99;
    private Throwable cause = new Throwable("detailed message of cause");

    public static void main(String argv[]) {
        System.out.println(new ThrowableExObjectnotifyAllIllegalMonitorStateException().run());
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = throwableExObjectnotifyAllIllegalMonitorStateException1();
        } catch (Exception e) {
            ThrowableExObjectnotifyAllIllegalMonitorStateException.res = ThrowableExObjectnotifyAllIllegalMonitorStateException.res - 20;
        }

        Thread t1 = new Thread(new ThrowableExObjectnotifyAllIllegalMonitorStateException11(1));
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

        if (result == 4 && ThrowableExObjectnotifyAllIllegalMonitorStateException.res == 58) {
            result = 0;
        }

        return result;
    }

    private int throwableExObjectnotifyAllIllegalMonitorStateException1() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        //
        //  final void notifyAll()
        try {
            cause.notifyAll();
            ThrowableExObjectnotifyAllIllegalMonitorStateException.res = ThrowableExObjectnotifyAllIllegalMonitorStateException.res - 10;
        } catch (IllegalMonitorStateException e2) {
            ThrowableExObjectnotifyAllIllegalMonitorStateException.res = ThrowableExObjectnotifyAllIllegalMonitorStateException.res - 1;
        }

        return result1;
    }

    private class ThrowableExObjectnotifyAllIllegalMonitorStateException11 implements Runnable {
        //  final void notifyAll()
        private int remainder;

        private ThrowableExObjectnotifyAllIllegalMonitorStateException11(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (cause) {
                try {
                    cause.notifyAll();
                    ThrowableExObjectnotifyAllIllegalMonitorStateException.res = ThrowableExObjectnotifyAllIllegalMonitorStateException.res - 40;
                } catch (IllegalMonitorStateException e2) {
                    ThrowableExObjectnotifyAllIllegalMonitorStateException.res = ThrowableExObjectnotifyAllIllegalMonitorStateException.res - 30;
                }
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n