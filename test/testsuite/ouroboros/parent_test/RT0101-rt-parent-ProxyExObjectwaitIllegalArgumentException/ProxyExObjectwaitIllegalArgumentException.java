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
 * -@TestCaseID: ProxyExObjectwaitIllegalArgumentException.java
 * -@TestCaseName: Exception in reflect Proxy: final void wait(*)
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create 2 private classes to implement Runnable, and call wait (millis), wait (millis, nanos) respectively in run
 * -#step2: Call the run of the use case and execute the start () method of the private class 1.
 * -#step3: Execute the start () method of private class 2
 * -#step4: Wait 1000 ms
 * -#step5: Confirm that all classes throw IllegalArgumentException
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ProxyExObjectwaitIllegalArgumentException.java
 * -@ExecuteClass: ProxyExObjectwaitIllegalArgumentException
 * -@ExecuteArgs:
 */

import java.lang.reflect.Proxy;
import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;

public class ProxyExObjectwaitIllegalArgumentException {
    static int res = 99;
    private MyProxy5 proxy = new MyProxy5(new MyInvocationHandler5());

    public static void main(String argv[]) {
        System.out.println(new ProxyExObjectwaitIllegalArgumentException().run());
    }

    private class ProxyExObjectwaitIllegalArgumentException21 implements Runnable {
        // IllegalArgumentException - if the value of timeout is negative.
        // final void wait(long millis)
        private int remainder;
        long millis = -1;

        private ProxyExObjectwaitIllegalArgumentException21(int remainder) {
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
                    ProxyExObjectwaitIllegalArgumentException.res = ProxyExObjectwaitIllegalArgumentException.res - 15;
                } catch (InterruptedException e1) {
                    ProxyExObjectwaitIllegalArgumentException.res = ProxyExObjectwaitIllegalArgumentException.res - 20;
                } catch (IllegalMonitorStateException e2) {
                    ProxyExObjectwaitIllegalArgumentException.res = ProxyExObjectwaitIllegalArgumentException.res - 10;
                } catch (IllegalArgumentException e3) {
                    ProxyExObjectwaitIllegalArgumentException.res = ProxyExObjectwaitIllegalArgumentException.res - 5;
                }
            }
        }
    }

    private class ProxyExObjectwaitIllegalArgumentException31 implements Runnable {
        // IllegalArgumentException - if the value of timeout is negative or the value of nanos is not in the range 0-999999.
        //
        // final void wait(long millis, int nanos)

        private int remainder;
        long millis = -2;
        int nanos = 10;

        private ProxyExObjectwaitIllegalArgumentException31(int remainder) {
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
                    ProxyExObjectwaitIllegalArgumentException.res = ProxyExObjectwaitIllegalArgumentException.res - 15;
                } catch (InterruptedException e1) {
                    ProxyExObjectwaitIllegalArgumentException.res = ProxyExObjectwaitIllegalArgumentException.res - 20;
                } catch (IllegalMonitorStateException e2) {
                    ProxyExObjectwaitIllegalArgumentException.res = ProxyExObjectwaitIllegalArgumentException.res - 10;
                } catch (IllegalArgumentException e3) {
                    ProxyExObjectwaitIllegalArgumentException.res = ProxyExObjectwaitIllegalArgumentException.res - 5;
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
        // final void wait(long millis)
        Thread t3 = new Thread(new ProxyExObjectwaitIllegalArgumentException21(3));
        // final void wait(long millis, int nanos)
        Thread t5 = new Thread(new ProxyExObjectwaitIllegalArgumentException31(5));
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
        t3.start();
        t5.start();
        sleep(1000);
        if (result == 2 && ProxyExObjectwaitIllegalArgumentException.res == 89) {
            result = 0;
        }

        return result;
    }
}

class MyProxy5 extends Proxy {
    MyProxy5(InvocationHandler h) {
        super(h);
    }

    InvocationHandler getInvocationHandlerField() {
        return h;
    }
}

class MyInvocationHandler5 implements InvocationHandler {
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
// ASSERT: scan-full 0\n