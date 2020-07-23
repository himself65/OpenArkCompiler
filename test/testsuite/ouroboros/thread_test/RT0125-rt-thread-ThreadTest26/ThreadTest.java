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
 *- @Title/Destination: test setUncaughtExceptionHandler().
 *- @Brief: see below
 * -#step1: 以"test thread group"为参数，创建一个ThreadGroup类的实例对象threadGroup；
 * -#step2: 以threadGroup、new ThreadRun("helloworld!")为参数，创建一个Thread类的实例对象thread，并且ThreadRun类实现了
 *          Runnable接口；
 * -#step3: 创建一个ExceptionHandler类的实例对象exceptionHandler，并以exceptionHandler为参数，调用thread的
 *          setUncaughtExceptionHandler()方法；
 * -#step4: 经判断得知实例对象exceptionHandler与thread.getUncaughtExceptionHandler()的返回值是相同的；
 * -#step5: 以"test thread group2"为参数，创建一个ThreadGroup类的实例对象threadGroup2；
 * -#step6: 以threadGroup2、new ThreadRun("helloworld!")为参数，创建一个Thread类的实例对象thread2，并且ThreadRun类实现了
 *          Runnable接口；
 * -#step7: 以null为参数，调用thread2的setUncaughtExceptionHandler()方法；
 * -#step8: 经判断得知实例对象threadGroup2与thread2.getUncaughtExceptionHandler()的返回值是相同的；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */
public class ThreadTest {
    public static void main(String[] args) throws Exception {
        //Test for setUncaughtExceptionHandler()
        ThreadGroup threadGroup = new ThreadGroup("test thread group");
        Thread thread = new Thread(threadGroup, new ThreadRun("helloworld!"));
        ExceptionHandler exceptionHandler = new ExceptionHandler();
        thread.setUncaughtExceptionHandler(exceptionHandler);
        System.out.println("should be same -- " + exceptionHandler.equals(thread.getUncaughtExceptionHandler()));

        //Test for setUncaughtExceptionHandler(null)
        ThreadGroup threadGroup2 = new ThreadGroup("test thread group2");
        Thread thread2 = new Thread(threadGroup2, new ThreadRun("helloworld!"));
        thread2.setUncaughtExceptionHandler(null);
        System.out.println("Thread's thread group is expected to be a handler -- "
                + threadGroup2.equals(thread2.getUncaughtExceptionHandler()));
        System.out.println("PASS");
    }

    private static class ThreadRun implements Runnable {
        private final String helloworld;

        public ThreadRun(String str) {
            helloworld = str;
        }

        public void run() {
            System.out.println(helloworld);
        }
    }

    static class ExceptionHandler implements Thread.UncaughtExceptionHandler {
        public boolean wasCalled = false;

        public void uncaughtException(Thread t, Throwable e) {
            wasCalled = true;
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full should be same -- true\nThread's thread group is expected to be a handler -- true\nPASS\n