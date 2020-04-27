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
 * -@TestCaseID: ThreadGetName
 *- @TestCaseName: Thread_ThreadGetName.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Get Thread Name.
 *- @Brief: see below
 * -#step1: 创建一个ThreadGetName类的实例对象threadGetName，且继承自Thread类；
 * -#step2: 调用threadGetName的start()方法启动该线程；
 * -#step3: 通过threadGetName的getName()方法返回的线程名与字符串"Thread-0"相同；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadGetName.java
 *- @ExecuteClass: ThreadGetName
 *- @ExecuteArgs:
 */

public class ThreadGetName extends Thread {
    public static void main(String[] args) {
        ThreadGetName threadGetName = new ThreadGetName();
        threadGetName.start();
        if (threadGetName.getName().equals("Thread-0")) {
            System.out.println(0);
        } else {
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0