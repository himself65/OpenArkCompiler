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
 * -@TestCaseID: ThreadDMCountStackFrames1
 *- @TestCaseName: Thread_ThreadDMCountStackFrames1.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Get the count of stack frames of a suspended thread by calling method countStackFrames(), the
 *                      count should be >= 0.
 *- @Brief: see below
 * -#step1: Create a thread instance.
 * -#step2: Override method run() and call suspend() in it.
 * -#step3: Start the thread.
 * -#step4: Get the count of stack frames of a suspended thread by calling method countStackFrames()
 * -#step5: Check the count of stack frames of a suspended thread Greater than or equal to 0.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadDMCountStackFrames1.java
 *- @ExecuteClass: ThreadDMCountStackFrames1
 *- @ExecuteArgs:
 */

public class ThreadDMCountStackFrames1 extends Thread{
    static int eCnt = 0;

    public static void main(String[] args) {
        ThreadDMCountStackFrames1 thread_obj = new ThreadDMCountStackFrames1();
        thread_obj.start();
        try {
            sleep(50);
        } catch (InterruptedException e1) {
            System.err.println(e1);
        }

        if (thread_obj.countStackFrames() >= 0 && eCnt == 1) {
            System.out.println(0);
            return;
        }

        thread_obj.stop();
        System.out.println(2);
    }

    public void run() {
        synchronized (this) {
            try {
                this.suspend();
            } catch (UnsupportedOperationException e2) {
                eCnt ++;
            }
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n