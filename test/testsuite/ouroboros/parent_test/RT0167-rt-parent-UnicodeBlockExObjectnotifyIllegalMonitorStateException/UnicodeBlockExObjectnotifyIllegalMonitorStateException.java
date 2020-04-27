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
 * -@TestCaseID: UnicodeBlockExObjectnotifyIllegalMonitorStateException.java
 * -@TestCaseName: Exception in Character/UnicodeBlock:  final void notify()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1:Prepare current thread is not the owner of the object's monitor
 * -#step2:Test UnicodeBlock api notifyAll extends from Object
 * -#step3:Throw IllegalMonitorStateException
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: UnicodeBlockExObjectnotifyIllegalMonitorStateException.java
 * -@ExecuteClass: UnicodeBlockExObjectnotifyIllegalMonitorStateException
 * -@ExecuteArgs:
 */

import java.lang.Character;

public class UnicodeBlockExObjectnotifyIllegalMonitorStateException {
    static int res = 99;
    static Character.UnicodeBlock unb1 = null;

    public static void main(String argv[]) {
        for (int cp = 0; cp <= 10; ++cp) {
            unb1 = Character.UnicodeBlock.of(cp);
        }
        System.out.println(new UnicodeBlockExObjectnotifyIllegalMonitorStateException().run());
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = unicodeBlockExObjectnotifyIllegalMonitorStateException1();
        } catch (Exception e) {
            UnicodeBlockExObjectnotifyIllegalMonitorStateException.res = UnicodeBlockExObjectnotifyIllegalMonitorStateException.res - 20;
        }

        Thread t1 = new Thread(new UnicodeBlockExObjectnotifyIllegalMonitorStateException11(1));
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

        if (result == 4 && UnicodeBlockExObjectnotifyIllegalMonitorStateException.res == 58) {
            result = 0;
        }

        return result;
    }

    private int unicodeBlockExObjectnotifyIllegalMonitorStateException1() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        // final void notify()
        try {
            unb1.notify();
            UnicodeBlockExObjectnotifyIllegalMonitorStateException.res = UnicodeBlockExObjectnotifyIllegalMonitorStateException.res - 10;
        } catch (IllegalMonitorStateException e2) {
            UnicodeBlockExObjectnotifyIllegalMonitorStateException.res = UnicodeBlockExObjectnotifyIllegalMonitorStateException.res - 1;
        }

        return result1;
    }

    private class UnicodeBlockExObjectnotifyIllegalMonitorStateException11 implements Runnable {
        // final void notify()
        private int remainder;

        private UnicodeBlockExObjectnotifyIllegalMonitorStateException11(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (unb1) {
                try {
                    unb1.notify();
                    UnicodeBlockExObjectnotifyIllegalMonitorStateException.res = UnicodeBlockExObjectnotifyIllegalMonitorStateException.res - 40;
                } catch (IllegalMonitorStateException e2) {
                    UnicodeBlockExObjectnotifyIllegalMonitorStateException.res = UnicodeBlockExObjectnotifyIllegalMonitorStateException.res - 30;
                }
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n