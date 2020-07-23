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
 * -@TestCaseID: MainThreadTest
 *- @TestCaseName: Thread_MainThreadTest.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Test to see if the main thread is in its thread group.
 *- @Brief: see below
 * -#step1: 通过Thread.currentThread().getThreadGroup()获取当前线程组对象。
 * -#step2: 调用activeCount()获取当前活跃的线程数目n。
 * -#step3: 创建一个n大小的线程组ts，调用enumerate(Thread[] tarray)复制当前的线程组到ts。
 * -#step4: 遍历线程组。main线程确认在遍历的线程组中。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: MainThreadTest.java
 *- @ExecuteClass: MainThreadTest
 *- @ExecuteArgs:
 */

public class MainThreadTest {
    public static void main(String[] args) {
        ThreadGroup tg = Thread.currentThread().getThreadGroup();
        int n = tg.activeCount();
        Thread[] ts = new Thread[n];
        int m = tg.enumerate(ts);
        for (int i = 0; i < ts.length; i++) {
            if (Thread.currentThread() == ts[i]) {
                System.out.println(0);
                return;
            }
        }
        throw new RuntimeException("Current thread is not in its own thread group!");
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n