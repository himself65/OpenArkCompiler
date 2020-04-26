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
 * -@TestCaseID: ThreadStatejoin17
 *- @TestCaseName: Thread_ThreadStatejoin17.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Negative input for join(long millis, int nanos).
 *- @Brief: see below
 * -#step1: 调用Thread()来构造对象实例。
 * -#step2：调用start()启动线程。
 * -#step3：构造参数millis为负数，nanos为正整数，调用join(long millis, int nanos)等待线程结束，确认抛出
 *          IllegalArgumentException异常。
 * -#step4：构造参数millis为正整数，nanos为负数，调用join(long millis, int nanos)等待线程结束，确认抛出
 *          IllegalArgumentException异常。
 * -#step5：构造参数millis为0，nanos为0，调用join(long millis, int nanos)等待线程结束，确认线程能正常运行。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadStatejoin17.java
 *- @ExecuteClass: ThreadStatejoin17
 *- @ExecuteArgs:
 */

public class ThreadStatejoin17 extends Thread {
    static int i = 0;
    static int eCount = 0;

    public static void main(String[] args) {
        ThreadStatejoin17 tj = new ThreadStatejoin17();
        tj.start();

        long[] invalidMillis = new long[]{-2l, 2l};
        int[] invalidNanos = new int[]{2, -2};

        for (int j = 0; j < invalidMillis.length; j++) {
            try {
                tj.join(invalidMillis[j], invalidNanos[j]);
            } catch (InterruptedException e) {
                System.out.println(e);
            } catch (IllegalArgumentException ee) {
                eCount++;
            }
        }

        try {
            tj.join(0, 0);
        } catch (InterruptedException e) {
            System.out.println(e);
        } catch (IllegalArgumentException ee) {
            eCount = eCount + 10;
        }

        if (i == 1 && eCount == invalidMillis.length) {
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