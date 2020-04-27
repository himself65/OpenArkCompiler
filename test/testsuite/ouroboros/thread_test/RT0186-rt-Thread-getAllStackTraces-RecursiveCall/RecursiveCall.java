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
 * -@TestCaseID: getAllStackTrace_RecursiveCall
 *- @TestCaseName: RecursiveCall.java
 *- @RequirementName: Unwind
 *- @Title/Destination: Generified basic unit test of Thread.getAllStackTraces()
 *- @Brief: see below
 * -#step1： 创造一个线程组，长度为1000，每个线程都有相同同步锁，解锁后等待10ms，
 * -#step2： ThreadOne类，该类均继承Thread类，分别定义run函数，会调用wait()方法依次调用A,B,C方法，并行执行线程组。
 * -#step3： 等待所有线程正常结束
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: RecursiveCall.java
 *- @ExecuteClass: RecursiveCall
 *- @ExecuteArgs:
 */


import java.util.Map;

public class RecursiveCall  extends Thread{
    private static String[] methodNames = {"run", "A", "B", "C", "Done"};
    private static Object obj = new Object();
    private static Thread[] tt = new Thread[1000];

    public static void main(String[] args) throws InterruptedException{

        for (int i = 0; i < tt.length; i++) {
            tt[i] = new ThreadOne();
        }
        for (int i = 0; i < tt.length; i++) {
            tt[i].start();
        }

        for (int i = 0; i < tt.length; i++) {
            try {
                tt[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

        }
        System.out.println(0);
    }


    static class ThreadOne extends Thread {
        @Override
        public void run() {
            System.out.println( );
            A();
        }

        private void A() {
            waitForDump();
            B();
        }

        private void B() {
            waitForDump();
            C();
        }

        private void C() {
            waitForDump();
            Done();
        }

        private void Done() {
            waitForDump();
        }
    }

    static private void waitForDump() {
        synchronized (obj) {
            try {
                obj.wait(10);
            } catch (InterruptedException e) {
                System.out.println("wait is interrupted");
            }
        }
    }


}



// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n