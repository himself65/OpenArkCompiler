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
 * -@TestCaseID: ThreadCountStackFrames
 *- @TestCaseName: Thread_ThreadCountStackFrames.java
 *- @RequirementName: Java Thread
 *- @Brief: see below
 * -#step1: 创建一个线程。
 * -#step2：在线程启动前检查，调用countStackFrames()确认返回0，调用getStackTrace()确认返回的长度为0。
 * -#step3：启动线程，等待线程结束。
 * -#step4：确认线程已经结束，调用countStackFrames()确认返回0，调用getStackTrace()确认返回的长度为0。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadCountStackFrames.java
 *- @ExecuteClass: ThreadCountStackFrames
 *- @ExecuteArgs:
 */

public class ThreadCountStackFrames {
    static int threadFinish = 0;

    public static void main(String[] args) {
        int result = 2;
        Thread thread = new Thread("ThreadCountStackFrames") {
            public void run() {
                try {
                    sleep(1000);
                } catch (Exception e) {
                    System.out.println(e);
                }
                ThreadCountStackFrames.threadFinish++;
            }
        };
        // Before the thread started.
        if (thread.countStackFrames() == 0 && thread.getStackTrace().length == 0) {
            result--;
        }

        thread.start();
        try {
            thread.join();
        } catch (Exception e) {
            System.out.println(e);
        }
        // After a thread finished.
        if (thread.countStackFrames() == 0 && thread.getStackTrace().length == 0
                && ThreadCountStackFrames.threadFinish == 1) {
            result--;
        }
        System.out.println(result);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n
