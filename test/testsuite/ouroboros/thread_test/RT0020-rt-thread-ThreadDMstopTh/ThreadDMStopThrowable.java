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
 * -@TestCaseID: ThreadDMStopThrowable
 *- @TestCaseName: Thread_ThreadDMStopThrowable.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Stop(Throwable) is Deprecated and throws UnsupportedOperationException in Android.
 *- @Brief: see below
 * -#step1: Create a thread instance.
 * -#step2: Override method run().
 * -#step3: Start the thread.
 * -#step4: Check that UnsupportedOperationException occurs when calling method stop(Throwable).
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadDMStopThrowable.java
 *- @ExecuteClass: ThreadDMStopThrowable
 *- @ExecuteArgs:
 */

public class ThreadDMStopThrowable extends Thread {
    public static void main(String[] args) {
        Throwable aaa = null;
        ThreadDMStopThrowable thread_obj = new ThreadDMStopThrowable();
        thread_obj.start();
        try {
            sleep(50);
        } catch (InterruptedException e1) {
            System.err.println(e1);
        }
        try {
            thread_obj.stop(aaa);
        } catch (UnsupportedOperationException e2) {
            System.out.println(0);
            return;
        }
        System.out.println(2);
    }

    public void run() {
        synchronized (this) {
            try {
                sleep(100);
            } catch (InterruptedException e4) {
                System.err.println(e4);
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0