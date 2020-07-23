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
 * -@TestCaseID: Daemon
 *- @TestCaseName: Thread_Daemon.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Test for premature destruction of daemon threadgroups
 *- @Brief: see below
 * -#step1: Create a thread group.
 * -#step2: Assign the new thread to thread group
 * -#step3: Create a new thread group,its parent thread group is tg.
 * -#step4: Change this thread group(tg) to a last backgrounder Thread Group
 * -#step5: Destroy the thread group by calling destroy()
 * -#step6: Check that child thread group was stop correctly.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: Daemon.java
 *- @ExecuteClass: Daemon
 *- @ExecuteArgs:
 */

public class Daemon {
    public static void main(String[] args) throws Exception {
        ThreadGroup tg = new ThreadGroup("madbot-threads");
        Thread myThread = new MadThread(tg, "mad");
        ThreadGroup aGroup = new ThreadGroup(tg, "ness");
        tg.setDaemon(true);
        if (tg.activeCount() != 0)
            throw new RuntimeException("activeCount");
        aGroup.destroy();
        if (tg.isDestroyed())
            throw new RuntimeException("destroy");
        try {
            Thread anotherThread = new MadThread(aGroup, "bot");
            throw new RuntimeException("illegal");
        } catch (IllegalThreadStateException itse) {
            // Correct result
        }
        System.out.println("Passed");
    }
}

class MadThread extends Thread {
    String name;

    MadThread(ThreadGroup tg, String name) {
        super(tg, name);
        this.name = name;
    }

    public void run() {
        System.out.println("me run " + name);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full Passed\n