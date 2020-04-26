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
 * -@TestCaseID: ThreadId
 *- @TestCaseName: Thread_ThreadId.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Use ThreadLocal to generate unique id for thread
 *- @Brief: see below
 * -#step1: Define a test class that to generate unique id for thread.
 * -#step2: Atomic integer containing the next thread ID to be assigned.
 * -#step3: Override initialValue().
 * -#step4: Check that the current thread's unique ID is correctly.
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadId.java
 *- @ExecuteClass: ThreadId
 *- @ExecuteArgs:
 */

import java.util.concurrent.atomic.AtomicInteger;

public class ThreadId {
    private static final AtomicInteger nextId = new AtomicInteger(0);

    private static final ThreadLocal<Integer> threadId =
            new ThreadLocal<Integer>() {
                @Override
                protected Integer initialValue() {
                    return nextId.getAndIncrement();
                }
            };

    public static int get() {
        return threadId.get();
    }

    public static void main(String[] args) {
        System.out.println(get());
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0