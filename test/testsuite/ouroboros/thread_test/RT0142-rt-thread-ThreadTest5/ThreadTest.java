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
 *- @Title/Destination: Test set/get DefaultUncaughtExceptionHandler and set/get DefaultUncaughtExceptionHandler(null).
 *- @Brief: see below
 * -#step1: 定义类ExceptionHandler实现Thread.UncaughtExceptionHandler，定义带参的构造方法。
 * -#step2: 通过new得到的类ExceptionHandler对象eh，通过setDefaultUncaughtExceptionHandler()设置类ExceptionHandler对象的处
 *          理程序。通过getDefaultUncaughtExceptionHandler()确认设置成功。
 * -#step3: 通过setDefaultUncaughtExceptionHandler()设置参数为null的处理程序。通过getDefaultUncaughtExceptionHandler()
 *          得到Thread.UncaughtExceptionHandler的对象。确认该对象为为null。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */

public class ThreadTest {
    public static void main(String[] args) throws Exception {
        // Test set/get DefaultUncaughtExceptionHandler
        ExceptionHandler eh = new ExceptionHandler();
        Thread.setDefaultUncaughtExceptionHandler(eh);
        Thread.UncaughtExceptionHandler ret1 = Thread.getDefaultUncaughtExceptionHandler();

        //Test set/get DefaultUncaughtExceptionHandler(null)
        Thread.setDefaultUncaughtExceptionHandler(null);
        Thread.UncaughtExceptionHandler ret2 = Thread.getDefaultUncaughtExceptionHandler();
        if (ret1 == eh && ret2 == null)
            System.out.println(0);
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
// ASSERT: scan-full 0\n