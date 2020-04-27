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
 * -@TestCaseID: ThreadGetId
 *- @TestCaseName: Thread_ThreadGetId.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: GetId() Returns the identifier of this Thread as a positive long number.
 *- @Brief: see below
 * -#step1: 创建一个ThreadGetId类的实例对象threadGetId，且ThreadGetId类继承自Thread类；
 * -#step2: 调用threadGetId的start()方法启动该线程；
 * -#step3: 通过threadGetId的getId()方法获取到其id，并赋值给Long类型的变量i；
 * -#step4: 确定变量i是Long类型的一个实例；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadGetId.java
 *- @ExecuteClass: ThreadGetId
 *- @ExecuteArgs:
 */

public class ThreadGetId extends Thread {
    public static void main(String[] args) {
        Long i;
        ThreadGetId threadGetId = new ThreadGetId();
        threadGetId.start();
        i = threadGetId.getId();
        if (i instanceof Long) {
            System.out.println(0);
        } else {
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0