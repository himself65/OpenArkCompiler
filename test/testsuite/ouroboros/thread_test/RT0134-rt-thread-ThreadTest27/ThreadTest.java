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
 *- @Title/Destination: Test set/get DefaultUncaughtExceptionHandler() and set/get DefaultUncaughtExceptionHandler
 *                      (null).
 *- @Brief: see below
 * -#step1: 创建一个ExceptionHandler类的实例对象exceptionHandler，并且ExceptionHandler类实现了
 *          Thread.UncaughtExceptionHandler接口；
 * -#step2: 以exceptionHandler为参数，调用Thread的setDefaultUncaughtExceptionHandler()方法；
 * -#step3: 经判断得知，exceptionHandler与Thread.getDefaultUncaughtExceptionHandler()的返回值相同；
 * -#step4: 以null为参数，调用Thread的setDefaultUncaughtExceptionHandler()方法；
 * -#step5: 经判断得知，Thread.getDefaultUncaughtExceptionHandler()的返回值为null；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */
public class ThreadTest {
    public static void main(String[] args) throws Exception {
        /**
         * Test set/get DefaultUncaughtExceptionHandler()
         */
        ExceptionHandler exceptionHandler = new ExceptionHandler();
        Thread.setDefaultUncaughtExceptionHandler(exceptionHandler);
        System.out.println("The default handler has been set, should print true -- " +
                exceptionHandler.equals(Thread.getDefaultUncaughtExceptionHandler()));


        /**
         * Test set/get DefaultUncaughtExceptionHandler(null)
         */
        Thread.setDefaultUncaughtExceptionHandler(null);
        System.out.println("Default handler should be null, and the test is -- " +
                Thread.getDefaultUncaughtExceptionHandler());
        System.out.println("PASS");
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
// ASSERT: scan The\s*default\s*handler\s*has\s*been\s*set\,\s*should\s*print\s*true\s*\-\-\s*true\s*Default\s*handler\s*should\s*be\s*null\,\s*and\s*the\s*test\s*is\s*\-\-\s*null\s*PASS