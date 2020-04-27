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
 * -@TestCaseID: ParameterExObjectwait.java
 *- @TestCaseName: Exception in reflect Parameter: final void wait(*)
 *- @TestCaseType: Function Test
 *- @RequirementName: 补充重写类的父类方法
 *- @Brief:
 * -#step1: Create three private classes to implement Runnable, and call wait (), wait (millis), wait (millis, nanos)
 *          in run respectively
 * -#step2: Call the run of the use case and execute the start () method of the private class 1.
 * -#step3: execute the start () method of the private class 2.
 * -#step4: execute the start () method of the private class 3.
 * -#step5: execute the start () method of the private class 1.
 * -#step6: Wait for 2 ~ 4 threads to finish running, confirm that wait ends successfully, no exception is thrown
 *- @Expect:0\n
 *- @Priority: High
 *- @Source: ParameterExObjectwait.java
 *- @ExecuteClass: ParameterExObjectwait
 *- @ExecuteArgs:
 */

import java.lang.reflect.Parameter;

public class ParameterExObjectwait {
    static int res = 99;
    private static Parameter[] param = null;

    public static void main(String argv[]) {
        try {
            param = ParameterExObjectwait.class.getMethod("sleep", new Class[]{int.class}).getParameters();
        } catch (NoSuchMethodException | SecurityException e) {
            e.printStackTrace();
        }
        System.out.println(new ParameterExObjectwait().run());
    }

    private class ParameterExObjectwait_11 implements Runnable {
        // final void wait()
        private int remainder;

        private ParameterExObjectwait_11(int remainder) {
            this.remainder = remainder;
        }

        public void run() {
            synchronized (param[0]) {
                param[0].notifyAll();
                try {
                    param[0].wait();
                    ParameterExObjectwait.res = ParameterExObjectwait.res - 15;
                } catch (InterruptedException e1) {
                    //System.out.println("111");
                    ParameterExObjectwait.res = ParameterExObjectwait.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    //System.out.println("222");
                    ParameterExObjectwait.res = ParameterExObjectwait.res - 10;
                }
            }
        }
    }

    private class ParameterExObjectwait_21 implements Runnable {
        // final void wait(long millis)
        private int remainder;
        long millis = 10;

        private ParameterExObjectwait_21(int remainder) {
            this.remainder = remainder;
        }

        public void run() {
            synchronized (param[0]) {
                param[0].notify();
                try {
                    param[0].wait(millis);
                    ParameterExObjectwait.res = ParameterExObjectwait.res - 15;
                } catch (InterruptedException e1) {
                    ParameterExObjectwait.res = ParameterExObjectwait.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    ParameterExObjectwait.res = ParameterExObjectwait.res - 10;
                } catch (IllegalArgumentException e3) {
                    ParameterExObjectwait.res = ParameterExObjectwait.res - 5;
                }
            }
        }
    }

    private class ParameterExObjectwait_31 implements Runnable {
        // final void wait(long millis, int nanos)

        private int remainder;
        long millis = 10;
        int nanos = 10;

        private ParameterExObjectwait_31(int remainder) {
            this.remainder = remainder;
        }

        public void run() {
            synchronized (param[0]) {
                param[0].notifyAll();
                try {
                    param[0].wait(millis, nanos);
                    ParameterExObjectwait.res = ParameterExObjectwait.res - 15;
                } catch (InterruptedException e1) {
                    ParameterExObjectwait.res = ParameterExObjectwait.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    ParameterExObjectwait.res = ParameterExObjectwait.res - 10;
                } catch (IllegalArgumentException e3) {
                    ParameterExObjectwait.res = ParameterExObjectwait.res - 5;
                }
            }
        }
    }

    public void sleep(int slpnum) {
        try {
            Thread.sleep(slpnum);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    private void waitFinish(Thread thread) {
        try {
            thread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public int run() {
        int result = 2; /*STATUS_FAILED*/
        //check api normal
        // final void wait()
        Thread t1 = new Thread(new ParameterExObjectwait_11(1));
        Thread t2 = new Thread(new ParameterExObjectwait_11(2));
        //final void wait(long millis)
        Thread t3 = new Thread(new ParameterExObjectwait_21(3));
        //final void wait(long millis, int nanos)
        Thread t5 = new Thread(new ParameterExObjectwait_31(5));
        t1.start();
        sleep(100);
        t3.start();
        sleep(100);
        t5.start();
        sleep(100);
        t2.start();
        t1.interrupt();
        t2.interrupt();

        waitFinish(t1);
        waitFinish(t3);
        waitFinish(t5);


        if (result == 2 && ParameterExObjectwait.res == 54) {
            result = 0;
        }

//        System.out.println("result: " + result);
//        System.out.println("ParameterExObjectwait.res: " + ParameterExObjectwait.res);
        return result;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n