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
 * -@TestCaseID: ModifierExObjectwait.java
 * -@TestCaseName: Exception in reflect Modifier: final void wait(*)
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create three private classes to implement Runnable, and call wait (), wait (millis), wait (millis, nanos)
 *          in run respectively
 * -#step2: Call the run of the use case and execute the start () method of the private class 1.
 * -#step3: execute the start () method of the private class 2.
 * -#step4: execute the start () method of the private class 3.
 * -#step5: execute the start () method of the private class 1.
 * -#step6: Wait for 2 ~ 4 threads to finish running, confirm that wait ends successfully, no exception is thrown
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ModifierExObjectwait.java
 * -@ExecuteClass: ModifierExObjectwait
 * -@ExecuteArgs:
 */

import java.lang.reflect.Modifier;

public class ModifierExObjectwait {
    static int res = 99;
    private Modifier mf2 = new Modifier();

    public static void main(String argv[]) {
        System.out.println(new ModifierExObjectwait().run());
    }

    private class ModifierExObjectwait11 implements Runnable {
        // final void wait()
        private int remainder;

        private ModifierExObjectwait11(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (mf2) {
                mf2.notifyAll();
                try {
                    mf2.wait();
                    ModifierExObjectwait.res = ModifierExObjectwait.res - 15;
                } catch (InterruptedException e1) {
                    ModifierExObjectwait.res = ModifierExObjectwait.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    ModifierExObjectwait.res = ModifierExObjectwait.res - 10;
                }
            }
        }
    }

    private class ModifierExObjectwait21 implements Runnable {
        // final void wait(long millis)
        private int remainder;
        long millis = 10;

        private ModifierExObjectwait21(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (mf2) {
                mf2.notify();
                try {
                    mf2.wait(millis);
                    ModifierExObjectwait.res = ModifierExObjectwait.res - 15;
                } catch (InterruptedException e1) {
                    ModifierExObjectwait.res = ModifierExObjectwait.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    ModifierExObjectwait.res = ModifierExObjectwait.res - 10;
                } catch (IllegalArgumentException e3) {
                    ModifierExObjectwait.res = ModifierExObjectwait.res - 5;
                }
            }
        }
    }

    private class ModifierExObjectwait31 implements Runnable {
        // final void wait(long millis, int nanos)

        private int remainder;
        long millis = 10;
        int nanos = 10;

        private ModifierExObjectwait31(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (mf2) {
                mf2.notifyAll();
                try {
                    mf2.wait(millis, nanos);
                    ModifierExObjectwait.res = ModifierExObjectwait.res - 15;
                } catch (InterruptedException e1) {
                    ModifierExObjectwait.res = ModifierExObjectwait.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    ModifierExObjectwait.res = ModifierExObjectwait.res - 10;
                } catch (IllegalArgumentException e3) {
                    ModifierExObjectwait.res = ModifierExObjectwait.res - 5;
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
     * join fun
     *
     * @param thread want join thread
     */
    private void waitFinish(Thread thread) {
        try {
            thread.join();
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
        Thread t1 = new Thread(new ModifierExObjectwait11(1));
        Thread t2 = new Thread(new ModifierExObjectwait11(2));
        // final void wait(long millis)
        Thread t3 = new Thread(new ModifierExObjectwait21(3));
        // final void wait(long millis, int nanos)
        Thread t5 = new Thread(new ModifierExObjectwait31(5));
        t1.setUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
            @Override
            public void uncaughtException(Thread t, Throwable e) {
                System.out.println(t.getName() + " : " + e.getMessage());
            }
        });
        t2.setUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {
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
        t3.start();
        sleep(1000);
        t5.start();
        sleep(1000);
        t2.start();
        t1.interrupt();
        t2.interrupt();

        waitFinish(t1);
        waitFinish(t3);
        waitFinish(t5);

        if (result == 2 && ModifierExObjectwait.res == 54) {
            result = 0;
        }

        return result;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n