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
 * -@TestCaseID: ThreadStateNotifyAll2
 *- @TestCaseName: Thread_ThreadStateNotifyAll2.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: if the current thread is not the owner of this object's monitor, notifyAll() throws
 *                      IllegalMonitorStateException
 *- @Brief: see below
 * -#step1: 定义一个Object类型的变量object，并赋初始值为"aa"；
 * -#step2: 尝试调用object的notifyAll()方法；
 * -#step3: step2中方法调用失败，会抛出IllegalMonitorStateException；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadStateNotifyAll2.java
 *- @ExecuteClass: ThreadStateNotifyAll2
 *- @ExecuteArgs:
 */

public class ThreadStateNotifyAll2 extends Thread {
    static Object object = "aa";

    public static void main(String[] args) {
        try {
            object.notifyAll();
            System.out.println(2);
        } catch (IllegalMonitorStateException e) {
            System.out.println(0);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n