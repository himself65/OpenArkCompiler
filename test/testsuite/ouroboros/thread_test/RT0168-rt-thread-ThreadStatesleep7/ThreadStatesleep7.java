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
 * -@TestCaseID: ThreadStatesleep7
 *- @TestCaseName: Thread_ThreadStatesleep7.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Sleep 0 millisecond time or 0 nanos, no exception threw.
 *- @Brief: see below
 * -#step1: 调用Thread()来构造对象实例。
 * -#step2：调用start()启动线程。
 * -#step3：构造参数millis为0，调用sleep(long millis)，确认线程能正常运行,无异常抛出。
 * -#step4：构造参数millis为0，nanos为0，调用sleep(long millis, int nanos)，确认确认线程能正常运行,无异常抛出。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadStatesleep7.java
 *- @ExecuteClass: ThreadStatesleep7
 *- @ExecuteArgs:
 */

public class ThreadStatesleep7 extends Thread {
    public static void main(String[] args) {
        ThreadStatesleep7 cls = new ThreadStatesleep7();
        cls.start();
        try {
            sleep(0);
            sleep(0, 0);
        } catch (InterruptedException e) {
            System.out.println("sleep is interrupted");
        } catch (IllegalArgumentException e1) {
            System.out.println(2);
            return;
        }
        System.out.println(0);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n