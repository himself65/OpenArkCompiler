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
 * -@TestCaseID: ThreadDMDestroy2
 *- @TestCaseName: Thread_ThreadDMdestory2.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Thread api: destroy(), added from JCK test case ${MAPLE_ROOT}/clinic/jck/JCK-runtime-8b/tests/
 *                      api/java_lang/Thread/index.html#destroy destroy001, destroy002, destroy003.
 *- @Brief: see below
 * -#step1: 直接调用new Thread().destroy()，确认会抛出UnsupportedOperationException。
 * -#step2：创建一个线程thread，调用start()启动，调用join()等待线程结束，调用destroy()，确认会抛出
 *          UnsupportedOperationException。
 * -#step3：创建一个线程st，调用start()启动，线程会执行sleep操作，调用destroy()，确认会抛出UnsupportedOperationException。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadDMdestory2.java
 *- @ExecuteClass: ThreadDMdestory2
 *- @ExecuteArgs:
 */

public class ThreadDMdestory2 {

    public static void main(String[] args) {
        System.out.println(run());
    }

    private static int run() {
        int exceptionCount = 0;

        // destroy001: test destroy() when the thread is not started
        try {
            new Thread().destroy();
            return 1;
        } catch (UnsupportedOperationException ok) {
            exceptionCount++;
        }

        // destroy002: test destroy() when the thread is finished
        Thread thread = new Thread();
        thread.start();
        try {
            thread.join();
        } catch (InterruptedException e) {
            System.out.println(e);
        }

        try {
            thread.destroy();
            return 2;
        } catch (UnsupportedOperationException ok) {
            exceptionCount++;
        }

        // destroy003: test destroy() when a thread is sleeping
        SleepingThread st = new SleepingThread();
        st.start();

        while (! st.started) {
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                // ignore any attempt to interrupt
                System.out.println(e);
            }
        }

        try {
            st.destroy();
            return 3;
        } catch (UnsupportedOperationException ok) {
            exceptionCount++;
        }
        st.checked=true;

        if (exceptionCount == 3) {
            return 0;
        }else{
            return 4;
        }
    }
}

class SleepingThread extends Thread {
    public volatile boolean started = false;
    public volatile boolean checked = false;

    public void run() {
        started = true;
        while (! checked) {
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                // ignore any attempt to interrupt
                System.out.println(e);
            }
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n