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
 * -@TestCaseID: ThreadConstructors10
 *- @TestCaseName: Thread_ThreadConstructors10.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Verifying that the Run method is not overridden in the thread object invokes the Run method in
 *                      the target object.
 *- @Brief: see below
 * -#step1: 创建一个ThreadConstructors10_a类的实例对象threadConstructors10_a；
 * -#step2: 以threadConstructors10_a为参数，创建一个ThreadConstructors10类的实例对象threadConstructors10；
 * -#step3: 调用threadConstructors10的start()方法启动该线程；
 * -#step4: 调用threadConstructors10的join()方法；
 * -#step5: 经判断得知ThreadConstructors10_a类的int类型的静态变量t的值经其内部调用run()方法后由0变为1；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadConstructors10.java
 *- @ExecuteClass: ThreadConstructors10
 *- @ExecuteArgs:
 */

class ThreadConstructors10_a implements Runnable {
    static int t = 0;

    public void run() {
        t++;
    }
}

public class ThreadConstructors10 extends Thread {
    public ThreadConstructors10(Runnable target) {
        super(target);
    }

    public static void main(String[] args) {
        ThreadConstructors10_a threadConstructors10_a = new ThreadConstructors10_a();
        ThreadConstructors10 threadConstructors10 = new ThreadConstructors10(threadConstructors10_a);
        threadConstructors10.start();
        try {
            threadConstructors10.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        if (ThreadConstructors10_a.t == 1) {
            System.out.println(0);
            return;
        }
        System.out.println(2);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n