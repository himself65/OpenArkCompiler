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
 * -@TestCaseID: StringMultiThreadsTest.java
 * -@TestCaseName: User Scene:multi-thread
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Inherit class Thread and override the run().
 * -#step2: Create ThreadGroup instance.
 * -#step3: Name threads with string value.
 * -#step4: Start all threads, check all threads run correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringMultiThreadsTest.java
 * -@ExecuteClass: StringMultiThreadsTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringMultiThreadsTest extends Thread {
    static int retu = 1;
    static int threadCount = 10;
    static int stringCount = 10;
    static int count = 0;

    public StringMultiThreadsTest(ThreadGroup group, String name) {
        super(group, name);
    }

    public void run() {
        String[] str = new String[stringCount];
        for (int i = 0; i < stringCount; i++) {
            str[i] = "ABC";
            count++;

            if (str[i] != "ABC") {
                retu++;
            }
        }
    }

    public static void main(String[] args) {
        System.out.println(test(args, System.out));
    }

    public static int test(String[] args, PrintStream out) {
        ThreadGroup thg = new ThreadGroup("threadg");
        String[] th = new String[threadCount];
        for (int i = 0; i < threadCount; i++) {
            th[i] = "thread" + i;
            new StringMultiThreadsTest(thg, th[i]).start();
        }

        try {
            sleep(200);
        } catch (InterruptedException e) {
        }

        if (retu == 1) {
            return 0;
        }
        return 2;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0