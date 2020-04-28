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
 * -@TestCaseID: ThreadGetContextClassLoader.java
 *- @TestCaseName: Thread_ThreadGetContextClassLoader.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Thread api: getContextClassLoader(), added from JCK test case ${MAPLE_ROOT}/clinic/jck/
 *                      JCK-runtime-8b/tests/api/java_lang/Thread/index.html#getContextClassLoader(), Thread4404.
 *- @Brief: see below
 * -#step1: 创建线程，调用getContextClassLoader()确认为父线程的ClassLoader上下文。
 * -#step2：启动线程，等待线程结束。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadGetContextClassLoader.java
 *- @ExecuteClass: ThreadGetContextClassLoader
 *- @ExecuteArgs:
 */

public class ThreadGetContextClassLoader {
    public static void main(String[] args) {
        Thread thr = new Thread();
        try {
            // If not set, the default is the ClassLoader context of the parent Thread.
            if (thr.getContextClassLoader() != Thread.currentThread().getContextClassLoader()) {
                System.out.println(2);
            }
        } finally {
            thr.start();
            try {
                thr.join();
            } catch (InterruptedException e) {
                System.out.println("Join is interrupted");
            }
        }
        System.out.println(0);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n