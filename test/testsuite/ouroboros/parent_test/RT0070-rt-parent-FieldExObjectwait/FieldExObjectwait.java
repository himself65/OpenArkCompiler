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
 * -@TestCaseID: Field/FieldExObjectwait.java
 * -@TestCaseName: Exception in reflect Field: final void wait(*)
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
 * -@Source: FieldExObjectwait.java
 * -@ExecuteClass: FieldExObjectwait
 * -@ExecuteArgs:
 */


import java.lang.reflect.Field;

public class FieldExObjectwait {
    static int res = 99;
    private Field[] cal = FieldExObjectwait.class.getDeclaredFields();
    private Field[] cal2 = FieldExObjectwait.class.getDeclaredFields();
    private Field[] cal3 = FieldExObjectwait.class.getDeclaredFields();

    public static void main(String argv[]) {
        System.out.println(new FieldExObjectwait().run());
    }

    private class FieldExObjectwait11 implements Runnable {
        // final void wait()

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (cal) {
                cal.notifyAll();
                try {
                    cal.wait();
                    FieldExObjectwait.res = FieldExObjectwait.res - 10;
                } catch (InterruptedException e1) {
                    FieldExObjectwait.res = FieldExObjectwait.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    FieldExObjectwait.res = FieldExObjectwait.res - 30;
                }
            }
        }
    }

    private class FieldExObjectwait12 implements Runnable {
        // final void wait(long millis)

        long millis = 10;

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (cal2) {
                cal2.notifyAll();
                try {
                    cal2.wait(millis);
                    FieldExObjectwait.res = FieldExObjectwait.res - 10;
                } catch (InterruptedException e1) {
                    FieldExObjectwait.res = FieldExObjectwait.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    FieldExObjectwait.res = FieldExObjectwait.res - 30;
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

    private class FieldExObjectwait13 implements Runnable {
        // final void wait(long millis, int nanos)

        long millis = 10;
        int nanos = 10;

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (cal3) {
                cal3.notifyAll();
                try {
                    cal3.wait(millis, nanos);
                    FieldExObjectwait.res = FieldExObjectwait.res - 10;
                } catch (InterruptedException e1) {
                    FieldExObjectwait.res = FieldExObjectwait.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    FieldExObjectwait.res = FieldExObjectwait.res - 30;
                }
            }
        }
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        // final void wait()
        Thread t1 = new Thread(new FieldExObjectwait11());
        Thread t2 = new Thread(new FieldExObjectwait11());
        // final void wait(long millis)
        Thread t3 = new Thread(new FieldExObjectwait12());
        // final void wait(long millis, int nanos)
        Thread t5 = new Thread(new FieldExObjectwait13());
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
        try {
            t1.join();
            t3.join();
            t5.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        if (result == 2 && FieldExObjectwait.res == 69) {
            result = 0;
        }
        t2.interrupt();
        return result;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n