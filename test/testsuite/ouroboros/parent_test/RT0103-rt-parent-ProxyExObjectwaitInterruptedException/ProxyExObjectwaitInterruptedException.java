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
 * -@TestCaseID: ProxyExObjectwaitInterruptedException.java
 * -@TestCaseName: Exception in reflect Proxy: final void wait(*)
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
 * -@Source: ProxyExObjectwaitInterruptedException.java
 * -@ExecuteClass: ProxyExObjectwaitInterruptedException
 * -@ExecuteArgs:
 */

import java.lang.reflect.Proxy;
import java.lang.reflect.Method;
import java.lang.reflect.InvocationHandler;

public class ProxyExObjectwaitInterruptedException {
    static int res = 99;
    private MyProxy8 proxy = new MyProxy8(new MyInvocationHandler7());

    public static void main(String argv[]) {
        System.out.println(new ProxyExObjectwaitInterruptedException().run());
    }

    private class ProxyExObjectwaitInterruptedException11 implements Runnable {
        // final void wait()
        private int remainder;

        private ProxyExObjectwaitInterruptedException11(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (proxy) {
                proxy.notifyAll();
                try {
                    proxy.wait();
                    ProxyExObjectwaitInterruptedException.res = ProxyExObjectwaitInterruptedException.res - 15;
                } catch (InterruptedException e1) {
                    ProxyExObjectwaitInterruptedException.res = ProxyExObjectwaitInterruptedException.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    ProxyExObjectwaitInterruptedException.res = ProxyExObjectwaitInterruptedException.res - 10;
                }
            }
        }
    }

    private class ProxyExObjectwaitInterruptedException21 implements Runnable {
        // final void wait(long millis)
        private int remainder;
        long millis = 10000;

        private ProxyExObjectwaitInterruptedException21(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (proxy) {
                proxy.notifyAll();
                try {
                    proxy.wait(millis);
                    ProxyExObjectwaitInterruptedException.res = ProxyExObjectwaitInterruptedException.res - 15;
                } catch (InterruptedException e1) {
                    ProxyExObjectwaitInterruptedException.res = ProxyExObjectwaitInterruptedException.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    ProxyExObjectwaitInterruptedException.res = ProxyExObjectwaitInterruptedException.res - 10;
                } catch (IllegalArgumentException e3) {
                    ProxyExObjectwaitInterruptedException.res = ProxyExObjectwaitInterruptedException.res - 5;
                }
            }
        }
    }

    private class ProxyExObjectwaitInterruptedException31 implements Runnable {
        // final void wait(long millis, int nanos)

        private int remainder;
        long millis = 10000;
        int nanos = 100;

        private ProxyExObjectwaitInterruptedException31(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (proxy) {
                proxy.notifyAll();
                try {
                    proxy.wait(millis, nanos);
                    ProxyExObjectwaitInterruptedException.res = ProxyExObjectwaitInterruptedException.res - 15;
                } catch (InterruptedException e1) {
                    ProxyExObjectwaitInterruptedException.res = ProxyExObjectwaitInterruptedException.res - 1;
                } catch (IllegalMonitorStateException e2) {
                    ProxyExObjectwaitInterruptedException.res = ProxyExObjectwaitInterruptedException.res - 10;
                } catch (IllegalArgumentException e3) {
                    ProxyExObjectwaitInterruptedException.res = ProxyExObjectwaitInterruptedException.res - 5;
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
        Thread t1 = new Thread(new ProxyExObjectwaitInterruptedException11(1));
        // final void wait(long millis)
        Thread t3 = new Thread(new ProxyExObjectwaitInterruptedException21(3));
        // final void wait(long millis, int nanos)
        Thread t5 = new Thread(new ProxyExObjectwaitInterruptedException31(5));
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

        if (result == 2 && ProxyExObjectwaitInterruptedException.res == 96) {
            result = 0;
        }

        return result;
    }
}

class MyProxy8 extends Proxy {
    MyProxy8(InvocationHandler h) {
        super(h);
    }

    InvocationHandler getInvocationHandlerField() {
        return h;
    }
}

class MyInvocationHandler7 implements InvocationHandler {
    /**
     * invoke test
     *
     * @param proxy  proxy test
     * @param method method for test
     * @param args   object[] for test
     * @return any implementation
     * @throws Throwable exception
     */
    public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
        return new Object(); // any implementation
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n