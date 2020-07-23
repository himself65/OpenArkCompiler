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
 * -@TestCaseID: StringContentEqualsTest.java
 * -@TestCaseName: Test String Method: boolean contentEquals(StringBuffer sb).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create parameters: sb is created by new StringBuffer(String str).
 * -#step3: Test method contentEquals(StringBuffer sb).
 * -#step4: Check the boolean result is correctly.
 * -#step5: Change str as One or more kinds of letters, numbers, special symbols/""/NoParam to repeat step2~4.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringContentEqualsTest.java
 * -@ExecuteClass: StringContentEqualsTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringContentEqualsTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringContentEqualsTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringContentEqualsTest_1() {
        String str1_1 = new String("abc123");
        String str1_2 = new String("******");
        String str1_3 = new String("ABc123");
        String str1_4 = new String("");
        String str1_5 = new String();

        String str2_1 = "abc123";
        String str2_2 = "******";
        String str2_3 = "ABc123";
        String str2_4 = "";

        test(str1_1);
        test(str1_2);
        test(str1_3);
        test(str1_4);
        test(str1_5);

        test(str2_1);
        test(str2_2);
        test(str2_3);
        test(str2_4);
    }

    private static void test(String str) {
        String test = "abc123";
        StringBuffer test1_1 = new StringBuffer(str);
        boolean result = test.contentEquals(test1_1);
        System.out.println(result);
    }
}




// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full true\nfalse\nfalse\nfalse\nfalse\ntrue\nfalse\nfalse\nfalse\n0\n