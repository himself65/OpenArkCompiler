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
 * -@TestCaseID: ThreadDMResume
 *- @TestCaseName: Thread_ThreadDMResume.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Use of deprecated method resume() throws UnsupportedOperationException in Android.
 *- @Brief: see below
 * -#step1: Create a thread instance.
 * -#step2: Override method run().
 * -#step3: Start the thread.
 * -#step4: Check that UnsupportedOperationException occurs when calling method resume().
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadDMResume.java
 *- @ExecuteClass: ThreadDMResume
 *- @ExecuteArgs:
 */

public class ThreadDMResume extends Thread {
    static int i = 0;

    public static void main(String[] args) {
        ThreadDMResume thread_obj = new ThreadDMResume();
        thread_obj.start();
        try {
            sleep(50);
        } catch (InterruptedException e1) {
            System.err.println(e1);
        }
        try {
            thread_obj.resume();
        } catch (UnsupportedOperationException e2) {
            System.out.println(0);
            return;
        }

        System.out.println(2);
    }

    public void run() {
        synchronized (this) {
            i++;
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0