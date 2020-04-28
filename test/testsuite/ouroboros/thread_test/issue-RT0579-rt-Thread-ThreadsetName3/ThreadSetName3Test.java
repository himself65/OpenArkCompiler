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
 * -@TestCaseID: ThreadSetName3Test
 *- @TestCaseName: Thread_ThreadSetName3Test.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Negative input for setName(String name).
 *- @Brief: see below
 * -#step1: 创建两个线程分别为testName1，testName2。
 * -#step2：调用start()启动两个线程。
 * -#step3：创建参数name为空字符串，两个线程分别都调用setName(String name)。
 * -#step4：通过getName()检查设置成功。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadSetName3Test.java
 *- @ExecuteClass: ThreadSetName3Test
 *- @ExecuteArgs:
 */

public class ThreadSetName3Test extends Thread {
    public static void main(String[] args) {
        ThreadSetName3Test testName1 = new ThreadSetName3Test();
        ThreadSetName3Test testName2 = new ThreadSetName3Test();
        testName1.start();
        testName1.setName("");
        testName2.start();
        testName2.setName("");

        if (testName1.getName().equals("")) {
            if (testName1.getName().equals("")) {
                System.out.println(0);
                return;
            }
        }
        System.out.println(2);
        return;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n