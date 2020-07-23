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
 * -@TestCaseID: StringEqualsTest.java
 * -@TestCaseName: Test String Method: boolean equals(Object anObject).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance, Create anObject as a String instance.
 * -#step2: Test method equals(Object anObject).
 * -#step3: Change instance/anObject as one or more kinds of letters, numbers, special symbols/""/NoParam to repeat
 *          step2.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringEqualsTest.java
 * -@ExecuteClass: StringEqualsTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringEqualsTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringEqualsTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringEqualsTest_1() {
        String str1_1 = new String("abc123");
        String str1_2 = new String("******");
        String str1_3 = new String("abc123");
        String str1_4 = new String("");
        String str1_5 = new String();
        String str1_6 = new String("ABc123");

        String str2_1 = "abc123";
        String str2_2 = "******";
        String str2_3 = "abc123";
        String str2_4 = "";
        String str2_5 = null;

        System.out.println(str1_1 + " equals " + str1_2 + " : " + str1_1.equals(str1_2));
        System.out.println(str1_1 + " equals " + str1_3 + " : " + str1_1.equals(str1_3));
        System.out.println(str1_1 + " equals " + str1_4 + " : " + str1_1.equals(str1_4));
        System.out.println(str1_1 + " equals " + str1_5 + " : " + str1_1.equals(str1_5));
        System.out.println(str1_1 + " equals " + str1_6 + " : " + str1_1.equals(str1_6));
        System.out.println(str1_2 + " equals " + str2_2 + " : " + str1_2.equals(str2_2));
        System.out.println(str2_1 + " equals " + str2_2 + " : " + str2_1.equals(str2_2));
        System.out.println(str2_1 + " equals " + str2_3 + " : " + str2_1.equals(str2_3));
        System.out.println(str2_1 + " equals " + str2_4 + " : " + str2_1.equals(str2_4));
        System.out.println(str2_1 + " equals " + str2_5 + " : " + str2_1.equals(str2_5));
        System.out.println(str1_1 + " equals " + str2_1 + " : " + str1_1.equals(str2_1));
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full abc123 equals ****** : false\nabc123 equals abc123 : true\nabc123 equals  : false\nabc123 equals  : false\nabc123 equals ABc123 : false\n****** equals ****** : true\nabc123 equals ****** : false\nabc123 equals abc123 : true\nabc123 equals  : false\nabc123 equals null : false\nabc123 equals abc123 : true\n0\n