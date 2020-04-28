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
 * -@TestCaseID: ThreadSetContextClassLoader2
 *- @TestCaseName: Thread_ThreadSetContextClassLoader2.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Use setContextClassLoader(ClassLoader cl) to change the class loader of Thread.
 *- @Brief: see below
 * -#step1: 创建本用例类的对象，通过getContextClassLoader()获取对应的类加载器。
 * -#step2：调用setContextClassLoader(ClassLoader cl)来设置类的ClassLoader。
 * -#step3：调用start()启动对应的实例对象，等待线程运行结束。
 * -#step4：确认对应的加载器成功运行。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadSetContextClassLoader2.java
 *- @ExecuteClass: ThreadSetContextClassLoader2
 *- @ExecuteArgs:
 */

public class ThreadSetContextClassLoader2 extends Thread {
    static int i;

    public static void main(String[] args) {
        ThreadSetContextClassLoader2 threadSetContextClassLoader2 = new ThreadSetContextClassLoader2();
        ClassLoader cl = threadSetContextClassLoader2.getContextClassLoader();
        threadSetContextClassLoader2.setContextClassLoader(cl.getParent());

        threadSetContextClassLoader2.start();
        try {
            threadSetContextClassLoader2.join();
        } catch (InterruptedException e) {
            System.out.println("Join is interrupted");
        }

        if (i == 1) {
            if (threadSetContextClassLoader2.getContextClassLoader() != cl) {
                System.out.println(0);
                return;
            }
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