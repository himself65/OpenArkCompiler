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
 * -@TestCaseID: ReflectPermission/ReflectPermissionExObjectwaitInterruptedException.java
 * -@TestCaseName: Exception in reflect ReflectPermission: final void wait(*)
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create three private classes to implement Runnable, run sampleField1 as a synchronization lock, Call wait(),
 *          wait (millis), wait (millis, nanos) respectively
 * -#step2: Call the run of the use case, execute the start () method of the private class 1, and wait for 100
 *          milliseconds to interrupt
 * -#step3: Execute the start () method of private class 2 and wait for 100 milliseconds to interrupt
 * -#step4: Execute the start () method of private class 3 and wait for 100 milliseconds to interrupt
 * -#step5: Confirm whether all classes are successfully interrupted and InterruptedException is successfully caught
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ReflectPermissionExObjectwaitInterruptedException.java
 * -@ExecuteClass: ReflectPermissionExObjectwaitInterruptedException
 * -@ExecuteArgs:
 */

import java.lang.reflect.ReflectPermission;

public class ReflectPermissionExObjectwaitInterruptedException {
    static int res = 99;
    private static String name = "name";
    private static String action = null;
    private static ReflectPermission RP = new ReflectPermission(name, action);

    public static void main(String argv[]) {
        System.out.println(new ReflectPermissionExObjectwaitInterruptedException().run());
    }

    private class ReflectPermissionExObjectwaitInterruptedException11 implements Runnable {
        // final void wait()

        private int remainder;

        private ReflectPermissionExObjectwaitInterruptedException11(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (RP) {
                RP.notifyAll();
                try {
                    RP.wait();
                    ReflectPermissionExObjectwaitInterruptedException.res = ReflectPermissionExObjectwaitInterruptedException.res - 15;
                } catch (InterruptedException e1) {
                    ReflectPermissionExObjectwaitInterruptedException.res = ReflectPermissionExObjectwaitInterruptedException.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    ReflectPermissionExObjectwaitInterruptedException.res = ReflectPermissionExObjectwaitInterruptedException.res - 10;
                }
            }
        }
    }

    private class ReflectPermissionExObjectwaitInterruptedException21 implements Runnable {
        // final void wait(long millis)
        private int remainder;
        long millis = 10000;

        private ReflectPermissionExObjectwaitInterruptedException21(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (RP) {
                RP.notifyAll();
                try {
                    RP.wait(millis);
                    ReflectPermissionExObjectwaitInterruptedException.res = ReflectPermissionExObjectwaitInterruptedException.res - 15;
                } catch (InterruptedException e1) {
                    ReflectPermissionExObjectwaitInterruptedException.res = ReflectPermissionExObjectwaitInterruptedException.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    ReflectPermissionExObjectwaitInterruptedException.res = ReflectPermissionExObjectwaitInterruptedException.res - 10;
                } catch (IllegalArgumentException e3) {
                    ReflectPermissionExObjectwaitInterruptedException.res = ReflectPermissionExObjectwaitInterruptedException.res - 5;
                }
            }
        }
    }

    private class ReflectPermissionExObjectwaitInterruptedException31 implements Runnable {
        // final void wait(long millis, int nanos)

        private int remainder;
        long millis = 10000;
        int nanos = 100;

        private ReflectPermissionExObjectwaitInterruptedException31(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (RP) {
                RP.notifyAll();
                try {
                    RP.wait(millis, nanos);
                    ReflectPermissionExObjectwaitInterruptedException.res = ReflectPermissionExObjectwaitInterruptedException.res - 15;
                } catch (InterruptedException e1) {
                    ReflectPermissionExObjectwaitInterruptedException.res = ReflectPermissionExObjectwaitInterruptedException.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    ReflectPermissionExObjectwaitInterruptedException.res = ReflectPermissionExObjectwaitInterruptedException.res - 10;
                } catch (IllegalArgumentException e3) {
                    ReflectPermissionExObjectwaitInterruptedException.res = ReflectPermissionExObjectwaitInterruptedException.res - 5;
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
        Thread t1 = new Thread(new ReflectPermissionExObjectwaitInterruptedException.ReflectPermissionExObjectwaitInterruptedException11(1));
        // final void wait(long millis)
        Thread t3 = new Thread(new ReflectPermissionExObjectwaitInterruptedException.ReflectPermissionExObjectwaitInterruptedException21(3));
        // final void wait(long millis, int nanos)
        Thread t5 = new Thread(new ReflectPermissionExObjectwaitInterruptedException.ReflectPermissionExObjectwaitInterruptedException31(5));
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
        sleep(100);
        t1.interrupt();
        sleep(100);
        t3.start();
        sleep(100);
        t3.interrupt();
        sleep(100);
        t5.start();
        sleep(100);
        t5.interrupt();
        sleep(100);

        if (result == 2 && ReflectPermissionExObjectwaitInterruptedException.res == 96) {
            result = 0;
        }

        return result;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n