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
 * -@TestCaseID: StringBuilderReplaceAndReverseTest.java
 * -@TestCaseName: Test Method StringBuffer replace(int start, int end, String str)/StringBuffer reverse() of
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      StringBuilder.
 * -@Brief:
 * -#case1
 * -#step1: Create StringBuilder instance.
 * -#step2: Create Parameters: start < end < instance.length, start/end coverage boundary value. str is String with some
  *         letters.
 * -#step3: Test method replace(int start, int end, String str), check return result is correctly.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~3.
 * -#case2
 * -#step1: Create StringBuilder instance.
 * -#step2: Test method reverse(), check return result is correctly.
 * -#step3: Change instance as One or more kinds of letters, numbers, special symbols/""/null to repeat step2.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringBuilderReplaceAndReverseTest.java
 * -@ExecuteClass: StringBuilderReplaceAndReverseTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringBuilderReplaceAndReverseTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringBuilderReplaceAndReverseTest_1();
            StringBuilderReplaceAndReverseTest_2();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringBuilderReplaceAndReverseTest_1() {
        StringBuilder strBuilder1_1 = new StringBuilder("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890" +
                "-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuilder strBuilder1_2 = new StringBuilder(" @!.&%");
        StringBuilder strBuilder1_3 = new StringBuilder("abc123abc");

        test1(strBuilder1_1);
        test1(strBuilder1_2);
        test1(strBuilder1_3);
    }

    public static void StringBuilderReplaceAndReverseTest_2() {
        StringBuilder strBuilder1_1 = new StringBuilder("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890" +
                "-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuilder strBuilder1_2 = new StringBuilder(" @!.&%");
        StringBuilder strBuilder1_3 = new StringBuilder("abc123abc");
        StringBuilder strBuilder1_4 = new StringBuilder("");
        StringBuilder strBuilder1_5 = new StringBuilder();

        test2(strBuilder1_1);
        test2(strBuilder1_2);
        test2(strBuilder1_3);
        test2(strBuilder1_4);
        test2(strBuilder1_5);
    }

    // Test method replace(int start, int end, String str).
    private static void test1(StringBuilder strBuilder) {
        System.out.println(strBuilder.replace(2, 6, "xyz"));
        // end = 0
        System.out.println(strBuilder.replace(0, 0, "xyz"));
        // start = 0 & end = strBuffer.length()
        System.out.println(strBuilder.replace(0, strBuilder.length(), "xyz"));
        // start = strBuffer.length() & end = strBuffer.length()
        System.out.println(strBuilder.replace(strBuilder.length(), strBuilder.length(), "xyz"));
    }

    // Test method reverse().
    private static void test2(StringBuilder strBuilder) {
        strBuilder.reverse();
        System.out.println(strBuilder);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full qwxyzuiop{}[]\\|asdfghjkl;:\'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nxyzqwxyzuiop{}[]\\|asdfghjkl;:\'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nxyz\nxyzxyz\n @xyz\nxyz @xyz\nxyz\nxyzxyz\nabxyzabc\nxyzabxyzabc\nxyz\nxyzxyz\n69x0MNBVCXZPOIUYTREWQLKJHGFDSA +_)(*&^%$#@!=-0987654321`~?/><.,mnbvcxz"\':;lkjhgfdsa|\\][}{poiuytrewq\n%&.!@ \ncba321cba\n\n\n0\n

