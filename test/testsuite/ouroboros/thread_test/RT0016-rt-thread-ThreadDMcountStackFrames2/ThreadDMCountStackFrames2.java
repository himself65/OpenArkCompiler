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
 * -@TestCaseID: ThreadDMCountStackFrames2
 *- @TestCaseName: Thread_ThreadDMCountStackFrames2.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Get the stack frame with no pending thread to report IllegalThreadStateException.
 *- @Brief: see below
 * -#step1: Create a thread instance.
 * -#step2: Override method run() without suspend().
 * -#step3: Start the thread.
 * -#step4: Check that IllegalThreadStateException occurs when calling method countStackFrames().
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadDMCountStackFrames2.java
 *- @ExecuteClass: ThreadDMCountStackFrames2
 *- @ExecuteArgs:
 */

public class ThreadDMCountStackFrames2 extends Thread {
    public static void main(String[] args) {
        ThreadDMCountStackFrames2 thread_obj = new ThreadDMCountStackFrames2();
        thread_obj.start();
        try {
            sleep(50);
        } catch (InterruptedException e1) {
            System.err.println(e1);
        }
        try {
            thread_obj.countStackFrames();
        } catch (IllegalThreadStateException e3) {
            System.out.println(2);
            return;
        }

        System.out.println(0);
    }

    public void run() {
        synchronized (this) {
            try {
                sleep(100);
            } catch (InterruptedException e2) {
                System.err.println(e2);
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n