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
 * -@TestCaseID: ThreadCountStackFramesTest
 *- @TestCaseName: Thread_ThreadCountStackFramesTest.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Check if countStackFrames throws IllegalThreadStateException when thread not suspended.
 *- @Brief: see below
 * -#step1: 创建Thread对象实例。
 * -#step2：调用start()启动对象实例。
 * -#step3：调用countStackFrames()确认无异常抛出。
 * -#step4：调用getStackTrace()，获取返回的长度为0。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadCountStackFramesTest.java
 *- @ExecuteClass: ThreadCountStackFramesTest
 *- @ExecuteArgs:
 */

public class ThreadCountStackFramesTest extends Thread {
    static int cnt = 0;
    static Object ob = "aa";

    public static void main(String[] args) {
        ThreadCountStackFramesTest thCountStackFramesTest = new ThreadCountStackFramesTest();
        thCountStackFramesTest.start();

        try {
            cnt = thCountStackFramesTest.countStackFrames();
        } catch (IllegalThreadStateException e) {
            System.out.println(2);
            return;
        }

        int j = thCountStackFramesTest.getStackTrace().length;
        if (j == cnt) {
            System.out.println(0);
            return;
        }

        System.out.println(2);
    }

    public void run() {
        synchronized (ob) {
            try {
                ob.wait(1000);
            } catch (InterruptedException e) {
                System.out.println("Wait is interrupted");
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n