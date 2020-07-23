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
 * -@TestCaseID: ThreadGetPriority
 *- @TestCaseName: Thread_ThreadGetPriority.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Get Thread Default Priority
 *- @Brief: see below
 * -#step1: 创建一个ThreadGetPriority类的实例对象threadGetPriority，且ThreadGetId类继承自Thread类；
 * -#step2: 调用threadGetPriority的start()方法启动该线程；
 * -#step3: 经判断得知通过threadGetPriority的getPriority()方法的返回值等于5；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadGetPriority.java
 *- @ExecuteClass: ThreadGetPriority
 *- @ExecuteArgs:
 */

public class ThreadGetPriority extends Thread {
    public static void main(String[] args) {
        ThreadGetPriority threadGetPriority = new ThreadGetPriority();
        threadGetPriority.start();
        if (threadGetPriority.getPriority() == 5) {
            System.out.println(0);
        } else {
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n