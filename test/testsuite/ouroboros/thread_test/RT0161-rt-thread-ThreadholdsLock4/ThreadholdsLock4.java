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
 * -@TestCaseID: ThreadholdsLock4
 *- @TestCaseName: Thread_ThreadholdsLock4.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Negative input for boolean holdsLock(Object obj).
 *- @Brief: see below
 * -#step1: 构造Thread实例。
 * -#step2：调用holdsLock(Object obj), 参数obj为null。
 * -#step3：确认抛出NullPointerException。
 * -#step4: 重复两次调用holdsLock(Object obj), 参数obj为"", 预期每次结果都返回false。
 * -#step5: 调用holdsLock(Object obj), 参数obj为new Object(), 预期结果返回false。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadholdsLock4.java
 *- @ExecuteClass: ThreadholdsLock4
 *- @ExecuteArgs:
 */

public class ThreadholdsLock4 extends Thread {
    static int eCount = 0;
    static int sCount = 0;

    public static void main(String[] args) {
        ThreadholdsLock4 tl = new ThreadholdsLock4();
        try {
            holdsLock(null);
        } catch (NullPointerException e) {
            eCount++;
        }

        if (!holdsLock("")) {
            if (!holdsLock("")) {
                if (!holdsLock(new Object())) {
                    sCount++;
                }
            }
        }

        if (sCount == 1 && eCount == 1) {
            System.out.println(0);
            return;
        }
        System.out.println(2);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n