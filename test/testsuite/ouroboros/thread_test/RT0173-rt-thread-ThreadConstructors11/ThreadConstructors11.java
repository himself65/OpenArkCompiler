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
 * -@TestCaseID: ThreadConstructors11
 *- @TestCaseName: Thread_ThreadConstructors11.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Negative input for Constructors Thread(Runnable target).
 *- @Brief: see below
 * -#step1: 构造参数target为null，调用Thread(Runnable target)来构造对象实例。
 * -#step2：调用start()启动线程。
 * -#step3：等待线程结束，确认线程能正常运行。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadConstructors11.java
 *- @ExecuteClass: ThreadConstructors11
 *- @ExecuteArgs:
 */

public class ThreadConstructors11 extends Thread {
    static int i = 0;

    public ThreadConstructors11(Runnable target) {
        super(target);
    }

    public static void main(String[] args) {
        ThreadConstructors11 test_illegal1 = new ThreadConstructors11(null);

        test_illegal1.start();
        try {
            test_illegal1.join();
        } catch (InterruptedException e) {
            System.out.println("Join is interrupted");
        }

        if (i == 1) {
            System.out.println(0);
            return;
        }
        System.out.println(2);
    }

    public void run() {
        i++;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n