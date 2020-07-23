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
 * -@TestCaseID: ThreadGroupEHdestroy
 *- @TestCaseName: Thread_ThreadGroupEHdestroy.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Check if destroy throws IllegalThreadStateException.
 *- @Brief: see below
 * -#step1: 通过new ThreadGroup(String name)构造新的实例tg。
 * -#step2: 通过Thread(ThreadGroup group, String name)分配新的thread对象。
 * -#step3：调用start()启动线程。
 * -#step4：对ThreadGroup对象tg调用destroy()，预计会抛出IllegalThreadStateException。
 * -#step5：调用join()等待线程运行结束。
 * -#step6：对ThreadGroup对象tg调用destroy()，预计会抛出IllegalThreadStateException。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadGroupEHdestroy.java
 *- @ExecuteClass: ThreadGroupEHdestroy
 *- @ExecuteArgs:
 */

public class ThreadGroupEHdestroy {
    static int cnt = 0;

    public static void main(String[] args) {
        ThreadGroup tg = new ThreadGroup("hi");
        Thread t = new Thread(tg, "hello");
        t.start();
        // Destroy a thread group which is not empty, throws IllegalThreadStateException
        try {
            tg.destroy();
        } catch (IllegalThreadStateException e) {
            cnt++;
        }
        try {
            t.join();
        } catch (InterruptedException ee) {
            System.out.println("Join is interrupted");
        }

        tg.destroy();
        // Destroy a thread group which is already destroyed, throws IllegalThreadStateException
        try {
            tg.destroy();
        } catch (IllegalThreadStateException e) {
            cnt += 2;
        }

        if (cnt == 3) {
            System.out.println(0);
            return;
        }
        System.out.println(2);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n