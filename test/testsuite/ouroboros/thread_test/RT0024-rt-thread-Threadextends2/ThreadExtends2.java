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
 * -@TestCaseID: ThreadExtends2
 *- @TestCaseName: Thread_ThreadExtends2.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Start different threads after inheritance
 *- @Brief: see below
 * -#step1: 定义含私有变量的类ThreadExtends1，继承于Thread类。
 * -#step2: 创建线程安全的run()方法。
 * -#step3: 利用new得到ThreadExtends1的对象，启动每一个对象线程。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadExtends2.java
 *- @ExecuteClass: ThreadExtends2
 *- @ExecuteArgs:
 */

class ThreadExtends2 extends Thread {
    private int ticket = 10;

    public static void main(String[] args) {
        ThreadExtends2 cls1 = new ThreadExtends2();
        ThreadExtends2 cls2 = new ThreadExtends2();
        ThreadExtends2 cls3 = new ThreadExtends2();
        try {
            cls1.start();
            cls2.start();
            cls3.start();
        } catch (Exception e) {
            System.out.println(2);
        }
        System.out.println(0);
    }

    public synchronized void run() {
        for (int i = 0; i < 20; i++) {
            if (this.ticket > 0) {
                this.ticket--;
            } else {
                break;
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0