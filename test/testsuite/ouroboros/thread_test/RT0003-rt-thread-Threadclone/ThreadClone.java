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
 * -@TestCaseID: ThreadClone
 *- @TestCaseName: Thread_ThreadClone.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Using clones in threads throws CloneNotSupportedException
 *- @Brief: see below
 * -#step1: 创建一个Thread类的实例对象threadClone；
 * -#step2: 调用threadClone的clone()方法会抛出CloneNotSupportedException；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ThreadClone.java
 *- @ExecuteClass: ThreadClone
 *- @ExecuteArgs:
 */

public class ThreadClone extends Thread {
    public static void main(String[] args) {
        ThreadClone threadClone = new ThreadClone();
        try {
            threadClone.clone();
            System.out.println(2);
        } catch (CloneNotSupportedException e) {
            System.out.println(0);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n