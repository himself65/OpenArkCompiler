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
 * -@TestCaseID: ThreadOutOfMemoryTest
 *- @TestCaseName: Thread_ThreadOutOfMemoryTest.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Check if thread creation reaches memory line will throw OutOfMemory.
 *- @Brief: see below
 * -#step1: 调用new Thread来创建线程，线程里会创建一个超大整数。
 * -#step2：使用循环反复启动该线程。
 * -#step3：检查线程是否会抛OutOfMemoryError异常并退出。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadOutOfMemoryTest.java
 *- @ExecuteClass: ThreadOutOfMemoryTest
 *- @ExecuteArgs:
 */
import java.io.PrintStream;

public class ThreadOutOfMemoryTest {
    private static Object s = new Object();
    private static int count = 0;
    private static int i = 0;

    public static void main(String[] args) {
        System.exit(run(args, System.out));
        System.out.println(2);
    }

    public static int run(String[] args, PrintStream out) {
        for (; ; ) {
            new Thread(new Runnable() {
                public void run() {
                    synchronized (s) {
                        try {
                            int[] y;
                            y = new int[50000 * 1000];
                            y[0]++;
                        } catch (OutOfMemoryError e2) {
                            i++;
                            System.err.println(e2);
                        }
                        count += 1;
//                        System.out.println("New thread #" + count);
                    }
                    for (; ; ) {
                        try {
                            Thread.sleep(1000);
                        } catch (InterruptedException e1) {
                            System.err.println(e1);
                        } catch (OutOfMemoryError e3) {
                            i++;
                            System.err.println(e3);
                        }
                    }
                }
            }).start();
            if (i != 0) {
//                System.out.println(i);
                break;
            }
        }
        System.out.println(0);
        return 0;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n