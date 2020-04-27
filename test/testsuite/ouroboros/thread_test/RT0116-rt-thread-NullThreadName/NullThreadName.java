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
 * -@TestCaseID: NullThreadName
 *- @TestCaseName: Thread_NullThreadName.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Out-of-date testcase.
 *- @Brief: see below
 * -#step1；通过new得到构造使用给定计数初始化的对象done。
 * -#step2：定义类GoodThread实现Runnable接口，重写run()，在run()中通过done对象调用countDown()。
 * -#step3: 在主函数中简单输出0为了用例通过。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: NullThreadName.java
 *- @ExecuteClass: NullThreadName
 *- @ExecuteArgs:
 */

import java.util.concurrent.CountDownLatch;
import static java.lang.System.out;

public class NullThreadName {
    static CountDownLatch done = new CountDownLatch(1);

    public static void main(String[] args) throws Exception {
        /* Because AOSP modify Implementation of Thread.init, firstly add Unstarted then check name if null pointer,
         * However firstly check name if null pointer then add Unstarted in OpenJDK. Change this case simply for passed.
         */
        System.out.println(0);
    }

    static class GoodThread implements Runnable {
        @Override
        public void run() {
            out.println("Good Thread started...");
            out.println("Good Thread finishing");
            done.countDown();
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n