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
 * -@TestCaseID: Threadenumerate2
 *- @TestCaseName: Thread_Threadenumerate2.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Negative input for static int enumerate(Thread[] tarray).
 *- @Brief: see below
 * -#step1: 创建Thread对象实例。
 * -#step2：调用start()启动对象实例。
 * -#step3: 构造参数tarray为null,调用enumerate(Thread[] tarray)。
 * -#step4：确认NullPointerException抛出。
 * -#step5: 构造参数tarray为new Thread[0],调用enumerate(Thread[] tarray)。
 * -#step6：确认返回为0。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: Threadenumerate2.java
 *- @ExecuteClass: Threadenumerate2
 *- @ExecuteArgs:
 */

public class Threadenumerate2 extends Thread {
    static int num = 0;
    static int eCount = 0;

    public static void main(String[] args) {
        Threadenumerate2 te = new Threadenumerate2();
        te.start();

        try {
            enumerate(null);
        } catch (NullPointerException e) {
            eCount++;
        }

        num = enumerate(new Thread[0]);
        if (num == 0 && eCount == 1) {
            System.out.println(0);
            return;
        }
        System.out.println(2);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0