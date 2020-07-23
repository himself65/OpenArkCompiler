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
 * -@TestCaseID: StringBuilderSubstringAndToStringTest.java
 * -@TestCaseName: Test Method:String substring(int start)/String substring(int start, int end)/String toString()
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
  *                     of StringBuilder.
 * -@Brief:
 * -#case1
 * -#step1: Create StringBuilder instance.
 * -#step2: Create Parameters: start < instance.length, start < end < instance.length, start and end coverage boundary
 *          value.
 * -#step3: Test method substring(int start, int end).
 * -#step4: Check the return result of substring is correctly.
 * -#step5: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~4.
 * -#case2
 * -#step1: Create StringBuilder instance.
 * -#step2: Create Parameters: 0 =< start < instance.length, start coverage boundary value.
 * -#step3: Test method substring(int start).
 * -#step4: Check the return result of substring is correctly.
 * -#step5: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~4.
 * -#case3
 * -#step1: Create StringBuilder instance by new StringBuilder(String stringLine).
 * -#step2: Test method toString().
 * -#step3: Check the return result of toString is correctly.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols/""/null to repeat step2-3.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringBuilderSubstringAndToStringTest.java
 * -@ExecuteClass: StringBuilderSubstringAndToStringTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringBuilderSubstringAndToStringTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringBuilderSubstringAndToStringTest_1();
            StringBuilderSubstringAndToStringTest_2();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringBuilderSubstringAndToStringTest_1() {
        StringBuilder strBuilder1_1 = new StringBuilder("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890" +
                "-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuilder strBuilder1_2 = new StringBuilder(" @!.&%()*");
        StringBuilder strBuilder1_3 = new StringBuilder("abc123abc");

        test1(strBuilder1_1);
        test1(strBuilder1_2);
        test1(strBuilder1_3);

        test2(strBuilder1_1);
        test2(strBuilder1_2);
        test2(strBuilder1_3);
    }

    public static void StringBuilderSubstringAndToStringTest_2() {
        StringBuilder strBuilder1_1 = new StringBuilder("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890" +
                "-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuilder strBuilder1_2 = new StringBuilder(" @!.&%");
        StringBuilder strBuilder1_3 = new StringBuilder("abc123abc");
        StringBuilder strBuilder1_4 = new StringBuilder("");
        StringBuilder strBuilder1_5 = new StringBuilder();

        test3(strBuilder1_1);
        test3(strBuilder1_2);
        test3(strBuilder1_3);
        test3(strBuilder1_4);
        test3(strBuilder1_5);
    }

    // Test method substring(int start, int end).
    private static void test1(StringBuilder strBuilder) {
        System.out.println(strBuilder.substring(2, 7));
        // Test start = 0 & end = strBuffer.length().
        System.out.println(strBuilder.substring(0, strBuilder.length()));
        // Test start = strBuffer.length().
        System.out.println(strBuilder.substring(strBuilder.length(), strBuilder.length()));
        // Test end = 0.
        System.out.println(strBuilder.substring(0, 0));
    }

    // Test method substring(int start).
    private static void test2(StringBuilder strBuilder) {
        System.out.println(strBuilder.substring(3));
        // Test start = 0.
        System.out.println(strBuilder.substring(0));
        // Test start = strBuffer.length().
        System.out.println(strBuilder.substring(strBuilder.length()));
    }

    // Test method toString().
    private static void test3(StringBuilder strBuilder) {
        System.out.println(strBuilder.toString());
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ertyu\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\n\n\n!.&%(\n @!.&%()*\n\n\nc123a\nabc123abc\n\n\nrtyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\n\n.&%()*\n @!.&%()*\n\n123abc\nabc123abc\n\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\n @!.&%\nabc123abc\n\n\n0\n

