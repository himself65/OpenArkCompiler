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
 * -@TestCaseID: ThreadStateWait6
 *- @TestCaseName: Thread_ThreadStateWait6.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: IllegalMonitorStateException is thrown by wait(long timeout, int nanos) if the current thread is
  *                     not the owner of the object's monitor.
 *- @Brief: see below
 * -#step1: 定义一个Object类型的变量object，并赋初始值为aa；
 * -#step2: 尝试调用object的wait()方法，参数为1000,100；
 * -#step3: step2中方法调用失败，会抛出IllegalMonitorStateException；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadStateWait6.java
 *- @ExecuteClass: ThreadStateWait6
 *- @ExecuteArgs:
 */

public class ThreadStateWait6 extends Thread {
    static Object object = "aa";

    public static void main(String[] args) {
        try {
            object.wait(1000, 100);
            System.out.println(2);
        } catch (InterruptedException e) {
            System.out.println(2);
        } catch (IllegalMonitorStateException e) {
            System.out.println(0);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0