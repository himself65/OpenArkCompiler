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
 * -@TestCaseID: ThreadConstructors5
 *- @TestCaseName: Thread_ThreadConstructors5.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Constructors Thread(String name), Thread.getName() return the name set in constructors.
 *- @Brief: see below
 * -#step1: 以good为参数，创建一个ThreadConstructors5类的实例对象threadConstructors5；
 * -#step2: 调用threadConstructors5的getName()方法，得知其返回值与字符串"good"相同；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadConstructors5.java
 *- @ExecuteClass: ThreadConstructors5
 *- @ExecuteArgs:
 */

public class ThreadConstructors5 extends Thread {
    public ThreadConstructors5(String name) {
        super(name);
    }

    public static void main(String[] args) {
        ThreadConstructors5 threadConstructors5 = new ThreadConstructors5("good");
        if (threadConstructors5.getName().equals("good")) {
            System.out.println(0);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n