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
 *- @TestCaseName: Thread_InitialValue.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Tests to see that a set nested in initialValue works OK.
 *- @Brief: see below
 * -#step1: 以new InitialValue()为参数，创建一个Thread类的实例对象thread，并且InitialValue类实现了Runnable接口；
 * -#step2: 调用thread的start()方法启动该线程；
 * -#step3: 调用thread的join()方法；
 * -#step4: InitialValue类内部的run()方法的逻辑是，创建一个MyLocal类的实例对象myLocal，并且MyLocal类继承自ThreadLocal类；
 *          调用myLocal的get()方法，并将其返回值赋值给string1；定义一个ThreadLocal类的变量other，调用其get()方法并将其返
 *          回值赋值给string2，当且仅当string2不等于null并且string2与字符串"Other"相同时，令boolean类型的静态全局变量
 *          passed等于true；
 * -#step5: 确认程序执行成功并且正常终止；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: InitialValue.java
 *- @ExecuteClass: InitialValue
 *- @ExecuteArgs:
 */
/*
 * Copyright (c) 2005, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

/*
 * @test
 * @bug     5025230
 * @summary Tests to see that a set nested in initialValue works OK
 * @author  Pete Soper
 */
public class InitialValue implements Runnable {
    static ThreadLocal<String> other;
    static boolean passed;

    public static void main(String[] args) {
        // Starting with Mustang the main thread already has an initialized
        // ThreadLocal map at this point, so test with a second thread.
        Thread thread = new Thread(new InitialValue());
        thread.start();
        try {
            thread.join();
        } catch (InterruptedException e) {
            throw new RuntimeException("Test Interrupted: failed");
        }
        if (!passed) {
            throw new RuntimeException("Test Failed");
        }
        System.out.println("pass");
    }

    public void run() {
        MyLocal myLocal = new MyLocal();
        // This should pick up the initial value
        String string1 = myLocal.get();
        // And this should pick up the other local in this thread's locals map
        String string2 = other.get();
        if ((string2 != null) && string2.equals("Other")) {
            // JMM guarantees this will be visible to
            // another thread joining with this thread's
            // termination: no need for this to be volatile.
            passed = true;
        }
    }

    public class MyLocal extends ThreadLocal<String> {
        protected String initialValue() {
            other = new ThreadLocal<String>();
            // This should reuse the map that the containing get() created
            // or visa versa (i.e. instead of a second map being created).
            other.set("Other");
            return "Initial";
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full pass\n