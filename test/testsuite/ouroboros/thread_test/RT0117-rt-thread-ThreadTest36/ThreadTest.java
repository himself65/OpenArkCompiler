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
 *- @Title/Destination: Test setPriority(), if set with value higher than threadGroup's maxPriority or a dead thread, do
  *                     nothing , if set with value out of legal range, throw exception.
 *- @Brief: see below
 * -#step1：定义静态私有类实现Runnable接口，含private final修饰的成员变量、ThreadRun()方法和run()方法。
 * -#step2：通过new得到线程组对象，通过setMaxPriority()设置线程组的最大优先级为9。
 * -#step3：通过new得到线程对象，通过setPriority()设置线程的优先级为最大优先级10，确认设置失败。
 * -#step4：通过new得到线程对象，得到当前线程对象的优先级。通过调用setPriority（）方法使新优先级大于最小优先级，确认设置成功。
 * -#step5：通过new得到又一个的线程对象，通过setPriority()方法设置比线程最大优先级更大的值，确认抛出IllegalArgumentException。
 * -#step6：通过new得到线程对象t4,启动对象线程，直到线程结束。相应的状态和优先级获取正确。通过调用setPriority（）方法设置线程的最
 *          大优先级，确认设置不生效。
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadTest.java
 *- @ExecuteClass: ThreadTest
 *- @ExecuteArgs:
 */

public class ThreadTest {
    public static void main(String[] args) throws Exception {
        /**
         * Verify the setPriority() method with new priority higher
         * than the maximum permitted priority for the thread's group.
         */
        ThreadGroup tg = new ThreadGroup("group1");
        int maxTGPriority = Thread.MAX_PRIORITY - 1;
        tg.setMaxPriority(maxTGPriority);
        Thread t = new Thread(tg, new ThreadRun("helloworld!"));
        t.setPriority(Thread.MAX_PRIORITY);
        System.out.println(maxTGPriority);
        System.out.println(tg.getMaxPriority() + " -- " + Thread.MAX_PRIORITY + " -- " + t.getPriority());

        /**
         * Verify the setPriority() method with new priority lower
         * than the current one.
         */
        Thread tt = new Thread();
        int p = tt.getPriority();
        int newPriority = p - 1;
        if (newPriority >= Thread.MIN_PRIORITY) {
            tt.setPriority(newPriority);
            System.out.println(newPriority + " === " + tt.getPriority());
        }

        /**
         * Verify the setPriority() method with new priority out of the legal range.
         */
        Thread ttt = new Thread();
        try {
            ttt.setPriority(Thread.MAX_PRIORITY + 2);
            System.out.println("Fail -- IllegalArgumentException should be thrown when new priority out of the legal range");
        } catch (IllegalArgumentException e) {
            System.out.println("IllegalArgumentException has be thrown when new priority out of the legal range");
        }

        /**
         * Verify the setPriority() method to a dead thread. do nothing
         */
        Thread t4 = new Thread(tg, new ThreadRun("helloworld2"));
        t4.start();
        t4.join();
        System.out.println(t4.getState());
        System.out.println(t4.getPriority());
        t4.setPriority(Thread.MAX_PRIORITY);
        System.out.println(t4.getPriority());
    }

    // Test for setPriority()
    private static class ThreadRun implements Runnable {
        private final String helloWorld;

        public ThreadRun(String str) {
            helloWorld = str;
        }

        public void run() {
            System.out.println(helloWorld);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 9\s*9\s*\-\-\s*10\s*\-\-\s*9\s*4\s*\=\=\=\s*4\s*IllegalArgumentException\s*has\s*be\s*thrown\s*when\s*new\s*priority\s*out\s*of\s*the\s*legal\s*range\s*helloworld2\s*TERMINATED\s*5\s*5