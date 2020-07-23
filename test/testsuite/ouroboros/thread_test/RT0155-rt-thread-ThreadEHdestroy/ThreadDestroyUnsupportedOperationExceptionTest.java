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
 * -@TestCaseID: ThreadDestroyUnsupportedOperationExceptionTest
 *- @TestCaseName: Thread_ThreadDestroyUnsupportedOperationExceptionTest.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Check if destroy() throws UnsupportedOperationException.
 *- @Brief: see below
 * -#step1: 创建Thread对象实例。
 * -#step2：调用start()启动对象实例。
 * -#step3: 调用destroy()方法。
 * -#step4：确认抛出UnsupportedOperationException。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadDestroyUnsupportedOperationExceptionTest.java
 *- @ExecuteClass: ThreadDestroyUnsupportedOperationExceptionTest
 *- @ExecuteArgs:
 */

public class ThreadDestroyUnsupportedOperationExceptionTest extends Thread {
    public static void main(String[] args) {
        ThreadDestroyUnsupportedOperationExceptionTest thread = new ThreadDestroyUnsupportedOperationExceptionTest();
        thread.start();

        try {
            thread.destroy();
        } catch (UnsupportedOperationException e) {
            System.out.println(0);
            return;
        } catch (Throwable ee) {
            System.out.println(1);
            return;
        }
        System.out.println(2);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n