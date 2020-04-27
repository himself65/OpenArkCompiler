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
 * -@TestCaseID: RefProCase/RefProcessor/src/DeadReferenceTest.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: RefProcessor  testcase：处理对象为死对象的弱引用
 *- @Brief:functionTest
 * -#step1：创建一个ReferenceQueue类的实例对象rq，并已null和rq为参数创建一个WeakReference的实例对象weakRp；
 * -#step2：新建一个ThreadRf线程，并开启此线程；
 * -#step3：让当前线程休眠2000ms；
 * -#step4：经判断得出weakRp.get()的返回值为null；
 * -#step5：判断rq.poll()的返回值为reference，直至全部判断完成；
 *- @Expect: ExpectResult\n
 *- @Priority: High
 *- @Source: DeadReferenceTest.java
 *- @ExecuteClass: DeadReferenceTest
 *- @ExecuteArgs:
 */

import java.lang.ref.*;

public class DeadReferenceTest {
    static int TEST_NUM = 1;
    static int judgeNum = 0;
    static Reference weakRp;
    static ReferenceQueue rq = new ReferenceQueue();

    public static void main(String[] args) throws Exception {
        judgeNum = 0;
        for (int i = 0; i < TEST_NUM; i++) {
            deadReferenceTest();
        }
        if (judgeNum == 0) {
            System.out.println("ExpectResult");
        }
    }

    static void setWeakRef() {
        weakRp = new WeakReference<Object>(null, rq);
    }

    static void deadReferenceTest() throws InterruptedException {
        Reference reference;

        setWeakRef();
        new Thread(new ThreadRf()).start();
        Thread.sleep(2000);
        if (weakRp.get() != null) {
            judgeNum++;
        }
        while ((reference = rq.poll()) != null) {
            if (!reference.getClass().toString().equals("class java.lang.ref.WeakReference")) {
                judgeNum++;
            }
        }
    }
}

class ThreadRf implements Runnable {
    public void run() {
        for (int i = 0; i < 60; i++) {
            WeakReference wr = new WeakReference(new Object());
            try {
                Thread.sleep(100);
            } catch (Exception e) {
                System.out.println(" sleep was Interrupted");
            }
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n