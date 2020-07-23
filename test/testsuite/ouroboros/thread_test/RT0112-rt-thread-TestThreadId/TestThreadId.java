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
 * -@TestCaseID: TestThreadId
 *- @TestCaseName: Thread_TestThreadId.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Negative input for Constructors Thread(Runnable target)
 *- @Brief: see below
 * -#step1: Define a test class.
 * -#step2: Create TestThreadId instance.
 * -#step3: Start all thread.
 * -#step4: Finish the thread by calling join().
 * -#step5: Returns the value in the current thread copy of this thread local variable
 * -#step6: Check that thread local variable exists.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: TestThreadId.java
 *- @ExecuteClass: TestThreadId
 *- @ExecuteArgs:
 */

import java.util.concurrent.atomic.AtomicInteger;

class ThreadId {
    private static final AtomicInteger nextId = new AtomicInteger(0);

    private static final ThreadLocal<Integer> threadId = new ThreadLocal<Integer>() {
                @Override
                protected Integer initialValue() {
                    return nextId.getAndIncrement();
                }
            };

    public static int get() {
        return threadId.get();
    }
}

public final class TestThreadId extends Thread {
    private static final int ITERATIONCOUNT = 50;
    private static final int THREADCOUNT = 50;

    private static ThreadId id = new ThreadId();

    private int value;

    public static void main(String[] args) throws Throwable {
        boolean[] check = new boolean[THREADCOUNT * ITERATIONCOUNT];

        TestThreadId[] u = new TestThreadId[THREADCOUNT];

        for (int i = 0; i < ITERATIONCOUNT; i++) {
            for (int t = 0; t < THREADCOUNT; t++) {
                u[t] = new TestThreadId();
                u[t].start();
            }

            for (int t = 0; t < THREADCOUNT; t++) {
                try {
                    u[t].join();
                } catch (InterruptedException e) {
                    throw new RuntimeException(
                            "TestThreadId: Failed with unexpected exception" + e);
                }
                try {
                    if (check[u[t].getIdValue()]) {
                        throw new RuntimeException(
                                "TestThreadId: Failed with duplicated id: " +
                                        u[t].getIdValue());
                    } else {
                        check[u[t].getIdValue()] = true;
                    }
                } catch (Exception e) {
                    throw new RuntimeException(
                            "TestThreadId: Failed with unexpected id value" + e);
                }
            }
        }
        System.out.println("0");
    }

    private synchronized int getIdValue() {
        return value;
    }

    public void run() {
        value = ThreadId.get();
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n