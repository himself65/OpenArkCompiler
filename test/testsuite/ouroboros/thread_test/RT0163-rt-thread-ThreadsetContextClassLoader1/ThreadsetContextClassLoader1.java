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
 * -@TestCaseID: ThreadsetContextClassLoader1
 *- @TestCaseName: Thread_ThreadsetContextClassLoader1.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Negative input for setContextClassLoader(ClassLoader cl).
 *- @Brief: see below
 * -#step1: 创建Thread实例。
 * -#step2：调用setContextClassLoader(ClassLoader cl)，参数cl为null。
 * -#step3：调用start()启动线程，等待线程结束。
 * -#step4：确认线程的run函数被正常调用。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadsetContextClassLoader1.java
 *- @ExecuteClass: ThreadsetContextClassLoader1
 *- @ExecuteArgs:
 */

public class ThreadsetContextClassLoader1 extends Thread {
    static int i = 0;

    public static void main(String[] args) {
        ThreadsetContextClassLoader1 t = new ThreadsetContextClassLoader1();
        t.setContextClassLoader(null);

        t.start();
        try {
            t.join();
        } catch (InterruptedException e) {
            System.out.println("Join is interrupted");
        }

        if (i == 1 && t.getContextClassLoader() == null) {
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
// ASSERT: scan-full 0\n