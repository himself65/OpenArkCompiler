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
 * -@TestCaseID: ThreadStateNotify1
 *- @TestCaseName: Thread_ThreadStateNotify1.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: notify() wakes up a single thread that is waiting on this object's monitor. If any threads are
 *                      waiting on this object, one of them is chosen randomly to be awakened.
 *- @Brief: see below
 * -#step1: 创建一个Object的对象并赋值给变量lock；
 * -#step2: 以lock为参数，创建两个ThreadA类的实例对象threadA、threadA2，并且ThreadA类继承自Thread类；
 * -#step3: 以lock为参数，创建SynNotifyMethodThreadA类的实例对象synNotifyMethodThreadA，并且SynNotifyMethodThreadA类继承
 *          自Thread类；
 * -#step4: 分别调用threadA、threadA2、synNotifyMethodThreadA的start()方法启动这三个线程；
 * -#step5: 让当前线程休眠1000ms；
 * -#step6: 确定在执行完ThreadA类和SynNotifyMethodThreadA类的内部的run()方法后，线程threadA、threadA2的状态有且仅有一个
 *          与枚举类型Thread.State.TIMED_WAITING相同，并且Service1类中定义的int类型的静态变量i和t都由0变为1；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadStateNotify1.java
 *- @ExecuteClass: ThreadStateNotify1
 *- @ExecuteArgs:
 */

class Service1 {
    static int i = 0;
    static int t = 0;

    public void testMethod(Object lock) {
        try {
            synchronized (lock) {
                lock.wait(3000);
                i++;
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public void syncNotifyMethod(Object lock) {
        try {
            Thread.sleep(100);
            synchronized (lock) {
                lock.notify();
                t++;
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}

class ThreadA extends Thread {
    private Object lock;

    public ThreadA(Object lock) {
        this.lock = lock;
    }

    public void run() {
        Service1 service = new Service1();
        service.testMethod(lock);
    }
}

class SynNotifyMethodThreadA extends Thread {
    private Object lock;

    public SynNotifyMethodThreadA(Object lock) {
        this.lock = lock;
    }

    public void run() {
        Service1 service = new Service1();
        service.syncNotifyMethod(lock);
    }
}

public class ThreadStateNotify1 {
    public static void main(String[] args) {
        Object lock = new Object();
        ThreadA threadA = new ThreadA(lock);
        ThreadA threadA2 = new ThreadA(lock);
        SynNotifyMethodThreadA synNotifyMethodThreadA = new SynNotifyMethodThreadA(lock);
        try {
            threadA.start();
            threadA2.start();
            synNotifyMethodThreadA.start();
            Thread.sleep(1000);
        } catch (Exception e) {
            e.printStackTrace();
        }
        if (!threadA.getState().equals(Thread.State.TIMED_WAITING) && Service1.i == 1 && Service1.t == 1) {
            System.out.println(0);
            return;
        }
        if (!threadA2.getState().equals(Thread.State.TIMED_WAITING) && Service1.i == 1 && Service1.t == 1) {
            System.out.println(0);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n