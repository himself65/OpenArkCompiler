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
 * -@TestCaseID: ThreadTest
 *- @TestCaseName: Thread_ThreadTest.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: test join(long millis) and join(long millis, int nanos) with valid and invalid value. when value
  *                     out of range, throws IllegalArgumentException.
 *- @Brief: see below
 * -#step1: 创建一个ThreadRunning类的实例对象threadRunning，并且ThreadRunning类继承自Thread类；
 * -#step2: 调用threadRunning的start()方法启动该线程；
 * -#step3: 尝试以-1为参数，调用threadRunning的join()方法；
 * -#step4: step3中会抛出IllegalArgumentException，此时将类变量threadRunning.stopWork赋值为true；
 * -#step5: 创建一个ThreadRunning类的实例对象threadRunning2，并且ThreadRunning类继承自Thread类；
 * -#step6: 调用threadRunning2的start()方法启动该线程；
 * -#step7: 以1000为参数，调用threadRunning2的join()方法；
 * -#step8: 程序执行成功并打印相关信息，此时将类变量threadRunning2.stopWork赋值为true；
 * -#step9: 创建一个ThreadRunning类的实例对象threadRunning3，并且ThreadRunning类继承自Thread类；
 * -#step10: 调用threadRunning3的start()方法启动该线程；
 * -#step11: 尝试以1000、1123456为参数，调用threadRunning3的join()方法；
 * -#step12: step11中会抛出IllegalArgumentException，此时将类变量threadRunning3.stopWork赋值为true；
 * -#step13: 创建一个ThreadRunning类的实例对象threadRunning4，并且ThreadRunning类继承自Thread类；
 * -#step14: 调用threadRunning4的start()方法启动该线程；
 * -#step15: 尝试以-2、123456为参数，调用threadRunning4的join()方法；
 * -#step16: step15中会抛出IllegalArgumentException，此时将类变量threadRunning4.stopWork赋值为true；
 * -#step17: 创建一个ThreadRunning类的实例对象threadRunning5，并且ThreadRunning类继承自Thread类；
 * -#step18: 调用threadRunning5的start()方法启动该线程；
 * -#step19: 以1000、123456为参数，调用threadRunning5的join()方法；
 * -#step20: 程序执行成功并打印相关信息，此时将类变量threadRunning5.stopWork赋值为true；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */
public class ThreadTest {
    public static void main(String[] args) throws Exception {
        //for  join(long millis)
        ThreadRunning threadRunning = new ThreadRunning();
        threadRunning.start();
        try {
            threadRunning.join(-1);
            System.out.println("join(millis) OK");
            threadRunning.stopWork = true;
        } catch (IllegalArgumentException e) {
            threadRunning.stopWork = true;
            System.out.println("in join(millis), millis is negative");
        }

        ThreadRunning threadRunning2 = new ThreadRunning();
        threadRunning2.start();
        try {
            threadRunning2.join(1000);
            System.out.println("join(millis) OK");
            threadRunning2.stopWork = true;
        } catch (IllegalArgumentException e) {
            threadRunning2.stopWork = true;
            System.out.println("in join(millis), millis is negative");
        }

        //for join(long millis, int nanos)
        ThreadRunning threadRunning3 = new ThreadRunning();
        threadRunning3.start();
        try {
            threadRunning3.join(1000, 1123456);
            System.out.println("join(millis, nanos) OK");
            threadRunning3.stopWork = true;
        } catch (IllegalArgumentException e) {
            threadRunning3.stopWork = true;
            System.out.println("in join(millis, nanos), nanosecond out of range");
        }

        ThreadRunning threadRunning4 = new ThreadRunning();
        threadRunning4.start();
        try {
            threadRunning4.join(-2, 123456);
            System.out.println("join(millis, nanos) OK");
            threadRunning4.stopWork = true;
        } catch (IllegalArgumentException e) {
            threadRunning4.stopWork = true;
            System.out.println("in join(millis, nanos), millis is negative");
        }

        ThreadRunning threadRunning5 = new ThreadRunning();
        threadRunning5.start();
        try {
            threadRunning5.join(1000, 123456);
            System.out.println("join(millis, nanos) OK");
            threadRunning5.stopWork = true;
        } catch (IllegalArgumentException e) {
            threadRunning5.stopWork = true;
            System.out.println("in join(millis, nanos), millis is negative or nanosecond out of range");
        }
        System.out.println("PASS");
    }

    static class ThreadRunning extends Thread {
        public volatile int i = 0;
        volatile boolean stopWork = false;

        ThreadRunning() {
            super();
        }

        public void run() {
            while (!stopWork) {
                i++;
                i--;
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full in join(millis), millis is negative\njoin(millis) OK\nin join(millis, nanos), nanosecond out of range\nin join(millis, nanos), millis is negative\njoin(millis, nanos) OK\nPASS\n