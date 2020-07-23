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
 * -@TestCaseID: ProxyExObjectnotifyIllegalMonitorStateException.java
 * -@TestCaseName: Exception in reflect Proxy:  final void notify()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create a private class to implement Runnable, call notify() inside run
 * -#step2: Call the run of the use case, execute method 1, and call notify() in the method
 * -#step3: Execute the start () method of the private class and wait 100 milliseconds
 * -#step4: Confirm method 1 throws an exception IllegalMonitorStateException, the class does not throw an exception
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ProxyExObjectnotifyIllegalMonitorStateException.java
 * -@ExecuteClass: ProxyExObjectnotifyIllegalMonitorStateException
 * -@ExecuteArgs:
 */

import java.lang.reflect.Proxy;
import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;

public class ProxyExObjectnotifyIllegalMonitorStateException {
    static int res = 99;
    private MyProxy2 proxy = new MyProxy2(new MyInvocationHandler1());

    public static void main(String argv[]) {
        System.out.println(new ProxyExObjectnotifyIllegalMonitorStateException().run());
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = proxyExObjectnotifyIllegalMonitorStateException1();
        } catch (Exception e) {
            ProxyExObjectnotifyIllegalMonitorStateException.res = ProxyExObjectnotifyIllegalMonitorStateException.res - 20;
        }

        Thread t1 = new Thread(new ProxyExObjectnotifyIllegalMonitorStateException11(1));
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

        if (result == 4 && ProxyExObjectnotifyIllegalMonitorStateException.res == 58) {
            result = 0;
        }

        return result;
    }

    private int proxyExObjectnotifyIllegalMonitorStateException1() {
        int result1 = 4; /*STATUS_FAILED*/
        // IllegalMonitorStateException - if the current thread is not the owner of the object's monitor.
        // final void notify()
        try {
            proxy.notify();
            ProxyExObjectnotifyIllegalMonitorStateException.res = ProxyExObjectnotifyIllegalMonitorStateException.res - 10;
        } catch (IllegalMonitorStateException e2) {
            ProxyExObjectnotifyIllegalMonitorStateException.res = ProxyExObjectnotifyIllegalMonitorStateException.res - 1;
        }

        return result1;
    }

    private class ProxyExObjectnotifyIllegalMonitorStateException11 implements Runnable {
        // final void notify()
        private int remainder;

        private ProxyExObjectnotifyIllegalMonitorStateException11(int remainder) {
            this.remainder = remainder;
        }

        /**
         * Thread run fun
         */
        public void run() {
            synchronized (proxy) {
                try {
                    proxy.notify();
                    ProxyExObjectnotifyIllegalMonitorStateException.res = ProxyExObjectnotifyIllegalMonitorStateException.res - 40;
                } catch (IllegalMonitorStateException e2) {
                    ProxyExObjectnotifyIllegalMonitorStateException.res = ProxyExObjectnotifyIllegalMonitorStateException.res - 30;
                }
            }
        }
    }
}

class MyProxy2 extends Proxy {
    MyProxy2(InvocationHandler h) {
        super(h);
    }

    InvocationHandler getInvocationHandlerField() {
        return h;
    }
}

class MyInvocationHandler1 implements InvocationHandler {
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