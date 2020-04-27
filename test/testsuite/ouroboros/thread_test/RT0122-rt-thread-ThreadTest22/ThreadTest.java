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
 * -@TestCaseID: ThreadTest
 *- @TestCaseName: Thread_ThreadTest.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: test for interrupt() -- Interrupt a newly created thread, Interrupting a thread that is not
 *                      alive need not have any effect.
 *- @Brief: see below
 * -#step1: 创建一个Thread类的实例对象thread，并调用其interrupt()方法中断此线程；
 * -#step2: 调用doSleep()方法，参数为600，返回boolean类型的值并赋值给expired；
 * -#step3: 调用thread的isInterrupted()方法，返回值记为result；
 * -#step4: 确认线程的状态是新建态，并且调用线程的isInterrupted()返回false，表明此时线程并未中断；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */
class ThreadTest {
    private static final long waitDuration = 3000;
    private static long waitTime = 0;

    private static boolean doSleep(int interval) {
        try {
            Thread.sleep(interval);
        } catch (InterruptedException e) {
            System.out.println("unexpected InterruptedException while sleeping");
        }
        waitTime -= interval;
        return waitTime <= 0;
    }

    // test for interrupt() -- Interrupt a newly created thread
    // Interrupting a thread that is not alive need not have any effect.
    /* If this thread is blocked in an invocation
     * of the wait(), wait(long), or wait(long, int) methods of the Object class, or
     * of the join(), join(long), join(long, int), sleep(long), or sleep(long, int), methods of this class,
     * then its interrupt status will be cleared and it will receive an InterruptedException.
     */
    public static void main(String[] args) {
        boolean expired = false;
        boolean result = false;
        Thread thread = new Thread();
        thread.interrupt();
        waitTime = waitDuration;
        while (!result && !expired) {
            expired = doSleep(600);
            result = thread.isInterrupted();
        }
        System.out.println("thread's state is -- " + thread.getState());
        System.out.println("isInterrupted() finally returns -- " + result);
        if (expired) {
            System.out.println("interrupt status has not changed to true");
        } else {
            System.out.println("*PASS*");
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan thread\'s\s*state\s*is\s*\-\-\s*NEW\s*isInterrupted\(\)\s*finally\s*returns\s*\-\-\s*false\s*interrupt\s*status\s*has\s*not\s*changed\s*to\s*true