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
 * -@TestCaseID: UnicodeBlockExObjectwaitInterruptedException.java
 * -@TestCaseName: Exception in Character/UnicodeBlock: final void wait(*)
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1:Test UnicodeBlock api wait extends from Object
 * -#step2:Thread is interruptted when wait
 * -#step3:Throw InterruptedException
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: UnicodeBlockExObjectwaitInterruptedException.java
 * -@ExecuteClass: UnicodeBlockExObjectwaitInterruptedException
 * -@ExecuteArgs:
 */

import java.lang.Character;

public class UnicodeBlockExObjectwaitInterruptedException {
    static int res = 99;
    private static Character.UnicodeBlock unb1 = null;

    public static void main(String argv[]) {
        for (int cp = 0; cp <= 10; ++cp) {
            unb1 = Character.UnicodeBlock.of(cp);
        }
        System.out.println(new UnicodeBlockExObjectwaitInterruptedException().run());
    }

    private class UnicodeBlockExObjectwaitInterruptedException11 implements Runnable {
        // final void wait()
        private int remainder;

        private UnicodeBlockExObjectwaitInterruptedException11(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (unb1) {
                unb1.notifyAll();
                try {
                    unb1.wait();
                    UnicodeBlockExObjectwaitInterruptedException.res = UnicodeBlockExObjectwaitInterruptedException.res - 15;
                } catch (InterruptedException e1) {
                    UnicodeBlockExObjectwaitInterruptedException.res = UnicodeBlockExObjectwaitInterruptedException.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    UnicodeBlockExObjectwaitInterruptedException.res = UnicodeBlockExObjectwaitInterruptedException.res - 10;
                }
            }
        }
    }

    private class UnicodeBlockExObjectwaitInterruptedException21 implements Runnable {
        // final void wait(long millis)
        private int remainder;
        long millis = 10000;

        private UnicodeBlockExObjectwaitInterruptedException21(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (unb1) {
                unb1.notifyAll();
                try {
                    unb1.wait(millis);
                    UnicodeBlockExObjectwaitInterruptedException.res = UnicodeBlockExObjectwaitInterruptedException.res - 15;
                } catch (InterruptedException e1) {
                    UnicodeBlockExObjectwaitInterruptedException.res = UnicodeBlockExObjectwaitInterruptedException.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    UnicodeBlockExObjectwaitInterruptedException.res = UnicodeBlockExObjectwaitInterruptedException.res - 10;
                } catch (IllegalArgumentException e3) {
                    UnicodeBlockExObjectwaitInterruptedException.res = UnicodeBlockExObjectwaitInterruptedException.res - 5;
                }
            }
        }
    }

    private class UnicodeBlockExObjectwaitInterruptedException31 implements Runnable {
        // final void wait(long millis, int nanos)

        private int remainder;
        long millis = 10000;
        int nanos = 100;

        private UnicodeBlockExObjectwaitInterruptedException31(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (unb1) {
                unb1.notifyAll();
                try {
                    unb1.wait(millis, nanos);
                    UnicodeBlockExObjectwaitInterruptedException.res = UnicodeBlockExObjectwaitInterruptedException.res - 15;
                } catch (InterruptedException e1) {
                    UnicodeBlockExObjectwaitInterruptedException.res = UnicodeBlockExObjectwaitInterruptedException.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    UnicodeBlockExObjectwaitInterruptedException.res = UnicodeBlockExObjectwaitInterruptedException.res - 10;
                } catch (IllegalArgumentException e3) {
                    UnicodeBlockExObjectwaitInterruptedException.res = UnicodeBlockExObjectwaitInterruptedException.res - 5;
                }
            }
        }
    }

    /**
     * sleep fun
     *
     * @param slpnum wait time
     */
    public void sleep(int slpnum) {
        try {
            Thread.sleep(slpnum);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        // check api normal
        // final void wait()
        Thread t1 = new Thread(new UnicodeBlockExObjectwaitInterruptedException11(1));
        // final void wait(long millis)
        Thread t3 = new Thread(new UnicodeBlockExObjectwaitInterruptedException21(3));
        // final void wait(long millis, int nanos)
        Thread t5 = new Thread(new UnicodeBlockExObjectwaitInterruptedException31(5));
        t1.setUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
            @Override
            public void uncaughtException(Thread t, Throwable e) {
                System.out.println(t.getName() + " : " + e.getMessage());
            }
        });
        t3.setUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
            @Override
            public void uncaughtException(Thread t, Throwable e) {
                System.out.println(t.getName() + " : " + e.getMessage());
            }
        });
        t5.setUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
            @Override
            public void uncaughtException(Thread t, Throwable e) {
                System.out.println(t.getName() + " : " + e.getMessage());
            }
        });
        t1.start();
        sleep(1000);
        t1.interrupt();
        sleep(1000);
        t3.start();
        sleep(1000);
        t3.interrupt();
        sleep(1000);
        t5.start();
        sleep(1000);
        t5.interrupt();
        sleep(1000);

        if (result == 2 && UnicodeBlockExObjectwaitInterruptedException.res == 96) {
            result = 0;
        }
        return result;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n