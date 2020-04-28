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
 * -@TestCaseID: ThreadStatejoin16
 *- @TestCaseName: Thread_ThreadStatejoin16.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Negative input for join(long millis)
 *- @Brief: see below
 * -#step1: 调用Thread()来构造对象实例。
 * -#step2：调用start()启动线程。
 * -#step3：构造参数millis为负数，调用join(long millis)等待线程结束，确认抛出IllegalArgumentException异常。
 * -#step4：构造参数millis为0，调用join(long millis)等待线程结束，确认线程能正常运行。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadStatejoin16.java
 *- @ExecuteClass: ThreadStatejoin16
 *- @ExecuteArgs:
 */

public class ThreadStatejoin16 extends Thread {
    static int i = 0;
    static int eCount = 0;

    public static void main(String[] args) {
        ThreadStatejoin16 tj = new ThreadStatejoin16();
        tj.start();

        try {
            tj.join(-2);
        } catch (InterruptedException e) {
            System.out.println(e);
        } catch (IllegalArgumentException ee) {
            eCount++;
        }

        try {
            tj.join(0);
        } catch (InterruptedException e) {
            System.out.println(e);
        } catch (IllegalArgumentException ee) {
            eCount = eCount + 2;
        }

        if (i == 1 && eCount == 1) {
            System.out.println(0);
            return;
        }
        System.out.println(2);
    }

    public void run() {
        i++;
        super.run();
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n