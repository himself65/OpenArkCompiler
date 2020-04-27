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
 * -@TestCaseID: ThreadStateNotifyAll1
 *- @TestCaseName: Thread_ThreadStateNotifyAll1.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: NotifyAll wakes up all threads that are waiting on this object's monitor.
 *- @Brief: see below
 * -#step1: 新建一个Object类型的对象lock；
 * -#step2: 以lock为参数，创建两个ThreadB类的实例对象threadB、threadB2，并且ThreadB类继承自Thread类；
 * -#step3: 以lock为参数，创建SynNotifyMethodThreadB类的实例对象synNotifyMethodThreadB，并且SynNotifyMethodThreadB类继承
 *          自Thread类；
 * -#step4: 分别调用threadB、threadB2、synNotifyMethodThreadB的start()方法启动这三个线程；
 * -#step5: 让当前线程休眠1000ms；
 * -#step6: 经判断得知，在执行完ThreadB类和SynNotifyMethodThreadB类的内部的run()方法后，线程threadB、threadB2的状态与枚
 *          举类Thread.State.WAITING均不相同，并且Service2类的int类型的静态变量i由0变为2，t由0变为1；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadStateNotifyAll1.java
 *- @ExecuteClass: ThreadStateNotifyAll1
 *- @ExecuteArgs:
 */

class Service2 {
    static int i = 0;
    static int t = 0;

    public void testMethod(Object lock) {
        try {
            synchronized (lock) {
                lock.wait();
                i++;
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public void synNotifyMethod(Object lock) {
        try {
            Thread.sleep(100);
            synchronized (lock) {
                lock.notifyAll();
                t++;
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}

class ThreadB extends Thread {
    private Object lock;

    public ThreadB(Object lock) {
        this.lock = lock;
    }

    public void run() {
        Service2 service = new Service2();
        service.testMethod(lock);
    }
}

class SynNotifyMethodThreadB extends Thread {
    private Object lock;

    public SynNotifyMethodThreadB(Object lock) {
        this.lock = lock;
    }

    public void run() {
        Service2 service = new Service2();
        service.synNotifyMethod(lock);
    }
}

public class ThreadStateNotifyAll1 {
    public static void main(String[] args) {
        Object lock = new Object();
        ThreadB threadB = new ThreadB(lock);
        ThreadB threadB2 = new ThreadB(lock);
        SynNotifyMethodThreadB synNotifyMethodThreadB = new SynNotifyMethodThreadB(lock);
        try {
            threadB.start();
            threadB2.start();
            synNotifyMethodThreadB.start();
            Thread.sleep(1000);
        } catch (Exception e) {
            e.printStackTrace();
        }
        if (!threadB.getState().equals(Thread.State.WAITING)) {
            if (!threadB2.getState().equals(Thread.State.WAITING) && Service2.i == 2 && Service2.t == 1) {
                System.out.println(0);
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n