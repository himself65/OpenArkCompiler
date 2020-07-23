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
 * -@TestCaseID: CharacterExObjectwaitInterruptedException.java
 * -@TestCaseName: Exception in Character: final void wait(*)
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
 * -@Source: CharacterExObjectwaitInterruptedException.java
 * -@ExecuteClass: CharacterExObjectwaitInterruptedException
 * -@ExecuteArgs:
 */

import java.lang.Thread;

public class CharacterExObjectwaitInterruptedException {
    static int res = 99;
    static char name = 'A';
    static Character sampleField1 = new Character(name);

    public static void main(String argv[]) throws NoSuchFieldException, SecurityException {
        System.out.println(new CharacterExObjectwaitInterruptedException().run());
    }

    private class CharacterExObjectwaitInterruptedException11 implements Runnable {
        /**
         * Thread run fun
         */
        public void run() {
            synchronized (sampleField1) {
                sampleField1.notifyAll();
                try {
                    sampleField1.wait();
                    CharacterExObjectwaitInterruptedException.res = CharacterExObjectwaitInterruptedException.res - 15;
                } catch (InterruptedException e1) {
                    CharacterExObjectwaitInterruptedException.res = CharacterExObjectwaitInterruptedException.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    CharacterExObjectwaitInterruptedException.res = CharacterExObjectwaitInterruptedException.res - 10;
                }
            }
        }
    }

    private class CharacterExObjectwaitInterruptedException21 implements Runnable {
        // final void wait(long millis)
        long millis = 10000;

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (sampleField1) {
                sampleField1.notifyAll();
                try {
                    sampleField1.wait(millis);
                    CharacterExObjectwaitInterruptedException.res = CharacterExObjectwaitInterruptedException.res - 15;
                } catch (InterruptedException e1) {
                    CharacterExObjectwaitInterruptedException.res = CharacterExObjectwaitInterruptedException.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    CharacterExObjectwaitInterruptedException.res = CharacterExObjectwaitInterruptedException.res - 10;
                } catch (IllegalArgumentException e3) {
                    CharacterExObjectwaitInterruptedException.res = CharacterExObjectwaitInterruptedException.res - 5;
                }
            }
        }
    }

    private class CharacterExObjectwaitInterruptedException31 implements Runnable {
        // final void wait(long millis, int nanos)
        long millis = 10000;
        int nanos = 100;

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (sampleField1) {
                sampleField1.notifyAll();
                try {
                    sampleField1.wait(millis, nanos);
                    CharacterExObjectwaitInterruptedException.res = CharacterExObjectwaitInterruptedException.res - 15;
                } catch (InterruptedException e1) {
                    CharacterExObjectwaitInterruptedException.res = CharacterExObjectwaitInterruptedException.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    CharacterExObjectwaitInterruptedException.res = CharacterExObjectwaitInterruptedException.res - 10;
                } catch (IllegalArgumentException e3) {
                    CharacterExObjectwaitInterruptedException.res = CharacterExObjectwaitInterruptedException.res - 5;
                }
            }
        }
    }

    /**
     * sleep fun
     * @param slpNum wait time
     */
    public void sleep(int slpNum) {
        try {
            Thread.sleep(slpNum);
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
        Thread t1 = new Thread(new CharacterExObjectwaitInterruptedException11());
        // final void wait(long millis)
        Thread t3 = new Thread(new CharacterExObjectwaitInterruptedException21());
        // final void wait(long millis, int nanos)
        Thread t5 = new Thread(new CharacterExObjectwaitInterruptedException31());
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
        if (result == 2 && CharacterExObjectwaitInterruptedException.res == 96) {
            result = 0;
        }

        return result;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n