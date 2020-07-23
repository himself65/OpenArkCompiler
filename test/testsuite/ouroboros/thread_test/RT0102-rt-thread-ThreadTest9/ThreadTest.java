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
 *- @Title/Destination: Verify currentThread() and toString()
 *- @Brief: see below
 * -#step1: 打印当前线程及当前线程名；
 * -#step2: 创建一个Thread类的实例对象thread，将thread.toString()赋值给info，thread.getName()赋值给name；
 * -#step3: 打印name的相关信息；
 * -#step4: 将new Integer(thread.getPriority()).toString()赋值给stringPriority，并打印其相关信息；
 * -#step5: 将thread.getThreadGroup().getName()赋值给groupName，并打印相关信息；
 * -#step6: 创建一个Thread类的实例对象thread2，并打印此线程及其相关信息；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */

public class ThreadTest {
    /**
     * Verify currentThread() and toString()
     */
    public static void main(String[] args) throws Exception {
        System.out.println(Thread.currentThread().toString());
        System.out.println(Thread.currentThread());
        System.out.println(Thread.currentThread().getName());

        Thread thread = new Thread();
        String info = thread.toString();
        String name = thread.getName();

        System.out.println("thread's name is -- " + name + " -- " + info.indexOf(name));
        String stringPriority = new Integer(thread.getPriority()).toString();
        System.out.println("thread's priority is -- " + stringPriority + " -- " + info.indexOf(","
                + stringPriority + ","));
        String groupName = thread.getThreadGroup().getName();
        System.out.println("thread's group is -- " + groupName + " -- " + info.indexOf(groupName));

        Thread thread2 = new Thread();
        System.out.println("thread name -- " +
                thread2.toString());
        System.out.println("thread group --- " + Thread.currentThread().getThreadGroup() +
                " --- " + thread2.getThreadGroup());
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full Thread[main,5,main]\nThread[main,5,main]\nmain\nthread's name is -- Thread-0 -- 7\nthread's priority is -- 5 -- 15\nthread's group is -- main -- 18\nthread name -- Thread[Thread-1,5,main]\nthread group --- java.lang.ThreadGroup[name=main,maxpri=10] --- java.lang.ThreadGroup[name=main,maxpri=10]\n