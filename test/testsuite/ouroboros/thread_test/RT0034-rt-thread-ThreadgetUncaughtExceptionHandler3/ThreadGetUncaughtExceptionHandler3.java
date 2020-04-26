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
 * -@TestCaseID: ThreadGetUncaughtExceptionHandler3
 *- @TestCaseName: Thread_ThreadGetUncaughtExceptionHandler3.java
 *- @RequirementName: Java Thread
 *- @Title/Destination: Cannot get exception in main thread after child thread ends(in Android, it does not return null).
 *- @Brief: see below
 * -#step1: 创建一个ThreadGetUncaughtExceptionHandler3类的实例对象threadGetUncaughtExceptionHandler3，且
 *          ThreadGetUncaughtExceptionHandler3类继承自Thread类；
 * -#step2: 通过threadGetUncaughtExceptionHandler3的setUncaughtExceptionHandler()方法设置其相关属性；
 * -#step3: 通过threadGetUncaughtExceptionHandler3的start()方法启动该线程；
 * -#step4: 调用threadGetUncaughtExceptionHandler3的join()方法；
 * -#step5: 将threadGetUncaughtExceptionHandler3的getUncaughtExceptionHandler()方法的返回值赋值给string；
 * -#step6: 经判断得知string中含有字符串"ThreadGetUncaughtExceptionHandler3"、"$"和"@"；
 *- @Expect: expected.txt
 *- @Priority: High
 *- @Source: ThreadGetUncaughtExceptionHandler3.java
 *- @ExecuteClass: ThreadGetUncaughtExceptionHandler3
 *- @ExecuteArgs:
 */

public class ThreadGetUncaughtExceptionHandler3 extends Thread{
    static String string;

    public void run() {
        System.out.println(3/0);
    }

    public static void main(String[] args) {
        ThreadGetUncaughtExceptionHandler3 threadGetUncaughtExceptionHandler3 = new
                ThreadGetUncaughtExceptionHandler3();
        threadGetUncaughtExceptionHandler3.setUncaughtExceptionHandler(new UncaughtExceptionHandler() {
            public void uncaughtException(Thread t, Throwable e) {
            }
        });
        threadGetUncaughtExceptionHandler3.start();
        try {
            threadGetUncaughtExceptionHandler3.join();
        } catch (InterruptedException e1) {
            System.out.println("Join is interrupted");
        }
        try {
            string = threadGetUncaughtExceptionHandler3.getUncaughtExceptionHandler().toString();
            if (string.indexOf("ThreadGetUncaughtExceptionHandler3") != -1) {
                if (string.toString().indexOf("$") != -1) {
                    if (string.toString().indexOf("@") != -1) {
                        System.out.println(0);
                        return;
                    }
                }
            }
        } catch (NullPointerException e2) {
            System.out.println(2);
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0