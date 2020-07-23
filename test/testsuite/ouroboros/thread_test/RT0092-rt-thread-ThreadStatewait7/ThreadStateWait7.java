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
 * -@TestCaseID: ThreadStateWait7
 *- @TestCaseName: Thread_ThreadStateWait7.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Verify the accuracy of the wait time
 *- @Brief: see below
 * -#step1: 创建一个ThreadStateWait7类的实例对象threadStateWait7，并且ThreadStateWait7类继承自Thread类；
 * -#step2: 调用threadStateWait7的start()方法启动该线程；
 * -#step3: 将当前系统时间赋值给long型变量j；
 * -#step4: 调用threadStateWait7的join()方法；
 * -#step5: 将当前系统时间赋值给long型变量k；
 * -#step6: 在ThreadStateWait7类内部的run()方法执行完之后（run()方法内以object为对象锁，并对object调用wait()方法令线程等
 *          待1000ms），经判断得知k与j的差值大于950且小于1050；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadStateWait7.java
 *- @ExecuteClass: ThreadStateWait7
 *- @ExecuteArgs:
 */

public class ThreadStateWait7 extends Thread {
    static Object object = "aa";

    public static void main(String[] args) {
        long j;
        long k;
        ThreadStateWait7 threadStateWait7 = new ThreadStateWait7();
        threadStateWait7.start();
        j = System.currentTimeMillis();
        try {
            threadStateWait7.join();
        } catch (InterruptedException ex) {
        }
        k = System.currentTimeMillis();
        if (k - j > 950 && k - j < 1050) {
            System.out.println(0);
        }
    }

    public void run() {
        synchronized (object) {
            try {
                object.wait(1000);
            } catch (InterruptedException e) {
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n