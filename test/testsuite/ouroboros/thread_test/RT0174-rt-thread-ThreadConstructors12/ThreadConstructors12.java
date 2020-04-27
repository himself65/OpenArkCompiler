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
 * -@TestCaseID: ThreadConstructors12
 *- @TestCaseName: Thread_ThreadConstructors12.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Negative input for Constructors Thread(Runnable target, String name).
 *- @Brief: see below
 * -#step1: 构造参数target为null，name为null,调用Thread(Runnable target, String name)来构造对象实例。
 * -#step2: 确认会抛出NullPointerException异常。
 * -#step3：构造参数target为null，name为"",调用Thread(Runnable target, String name)来构造对象实例test_illegal2。
 * -#step4: 构造参数target为null，name为new String(),调用Thread(Runnable target, String name)来构造对象实例
 *          test_illegal3.
 * -#step2：调用start()启动线程test_illegal2,test_illegal3。
 * -#step3：等待线程结束，确认两个线程都能正常运行。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadConstructors12.java
 *- @ExecuteClass: ThreadConstructors12
 *- @ExecuteArgs:
 */

public class ThreadConstructors12 extends Thread {
    static int i = 0;
    static int eCount = 0;

    public ThreadConstructors12(Runnable target, String name) {
        super(target, name);
    }

    public static void main(String[] args) {
        try {
            ThreadConstructors12 test_illegal1 = new ThreadConstructors12(null, null);
        } catch (NullPointerException e) {
            eCount++;
        }

        ThreadConstructors12 test_illegal2 = new ThreadConstructors12(null, "");
        ThreadConstructors12 test_illegal3 = new ThreadConstructors12(null, new String());

        test_illegal2.start();
        try {
            test_illegal2.join();
        } catch (InterruptedException e) {
            System.out.println("test_illegal2 join is interrupted");
        }

        test_illegal3.start();
        try {
            test_illegal3.join();
        } catch (InterruptedException e) {
            System.out.println("test_illegal3 join is interrupted");
        }

        if (i == 2 && eCount == 1) {
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