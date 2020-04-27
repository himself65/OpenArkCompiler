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
 * -@TestCaseID: ThreadTest
 *- @TestCaseName: Thread_ThreadTest.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: test for run(),  here t1 and t2 will be treated as normal object not thread object
 *- @Brief: see below
 * -#step1: 以t1、t2为参数，创建ThreadTest类的两个实例对象threadTest1、threadTest2，并且ThreadTest类继承自Thread类；
 * -#step2: 分别调用threadTest1、threadTest2的run()方法，其内的逻辑是当i < 5 时，让当前线程休眠500ms，之后打印当前线程的
 *          名字；
 * -#step3: 确定程序执行成功并正常终止；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */

class ThreadTest extends Thread {
    private String name;

    public ThreadTest(String name) {
        this.name = name;
    }

    public static void main(String[] args) {
        ThreadTest threadTest1 = new ThreadTest("t1");
        ThreadTest threadTest2 = new ThreadTest("t2");

        threadTest1.run();
        threadTest2.run();
    }

    public void run() {
        for (int i = 1; i < 5; i++) {
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                System.out.println(e);
            }
            System.out.println(name + " -- " + i);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan t1\s*\-\-\s*1\s*t1\s*\-\-\s*2\s*t1\s*\-\-\s*3\s*t1\s*\-\-\s*4\s*t2\s*\-\-\s*1\s*t2\s*\-\-\s*2\s*t2\s*\-\-\s*3\s*t2\s*\-\-\s*4