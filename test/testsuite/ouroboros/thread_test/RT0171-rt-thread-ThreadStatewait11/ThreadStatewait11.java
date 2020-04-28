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
 * -@TestCaseID: ThreadStatewait11
 *- @TestCaseName: Thread_ThreadStatewait11.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Invalid input for Object.wait(long timeout).
 *- @Brief: see below
 * -#step1: 调用ThreadStatewait11()来构造对象实例。
 * -#step2：调用start()启动线程。
 * -#step3: 构造参数timeout为0，调用wait(long timeout)，确认会抛出IllegalMonitorStateException.
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadStatewait11.java
 *- @ExecuteClass: ThreadStatewait11
 *- @ExecuteArgs:
 */

public class ThreadStatewait11 extends Thread {
    public static void main(String[] args) {
        ThreadStatewait11 threadWait = new ThreadStatewait11();
        threadWait.start();
        try {
            threadWait.wait(0);
        } catch (InterruptedException e) {
            System.out.println(2);
            return;
        } catch (IllegalMonitorStateException e1) {
            System.out.println(0);
            return;
        }
        System.out.println(2);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n