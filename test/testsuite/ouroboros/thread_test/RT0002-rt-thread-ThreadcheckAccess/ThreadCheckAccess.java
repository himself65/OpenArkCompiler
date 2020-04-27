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
 * -@TestCaseID: ThreadCheckAccess
 *- @TestCaseName: Thread_ThreadCheckAccess.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Thread.checkAccess() returns if the current running thread has permission to modify this thread.
 *- @Brief: see below
 * -#step1: 创建一个Thread的实例对象threadCheckAccess；
 * -#step2: 调用threadCheckAccess的checkAccess()方法，发现程序完整运行并未抛出任何异常；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadCheckAccess.java
 *- @ExecuteClass: ThreadCheckAccess
 *- @ExecuteArgs:
 */

public class ThreadCheckAccess extends Thread {
    public static void main(String[] args) {
        ThreadCheckAccess threadCheckAccess = new ThreadCheckAccess();
        try {
            threadCheckAccess.checkAccess();
        } catch (Exception e) {
            System.out.println(2);
            return;
        }
        System.out.println(0);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n