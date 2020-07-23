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
 * -@TestCaseID: StringBuilderConstructorsTest.java
 * -@TestCaseName: Test StringBuilder Constructors StringBuilder()/StringBuilder(CharSequence seq)/StringBuilder
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      (int capacity)/StringBuilder(String str).
 * -@Brief:
 * -#case1
 * -#step1: Create a String instance By new String(String str) and not new.
 * -#step2: Test Constructor StringBuilder(String instance).
 * -#step3: Check new StringBuilder instance is new correctly.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols/""/null to repeat step2~3.
 * -#case2
 * -#step1: Test new StringBuilder() and check the capacity() of instance is correctly.
 * -#step2: Change instance as One or more kinds of letters, numbers, special symbols/""/null to repeat step1.
 * -#case3
 * -#step1: Create capacity > 0 or capacity = 0, test new StringBuilder(int capacity), check the capacity() of instance
 *          is correctly.
 * -#case4
 * -#step1: Create seq Contains numbers, letters, special symbols, test StringBuilder(CharSequence seq), check new
 *          instance is correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringBuilderConstructorsTest.java
 * -@ExecuteClass: StringBuilderConstructorsTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringBuilderConstructorsTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringBuilderConstructorsTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringBuilderConstructorsTest_1() {
        String str1_1 = new String("abc123abc");
        String str1_2 = new String(" @!.&%()*");
        String str1_3 = new String("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ AS" +
                "DFGHJKLQWERTYUIOPZXCVBNM0x96");
        String str1_4 = new String("");
        String str1_5 = new String();

        String str2_1 = "abc123ABC";
        String str2_2 = " @!.&%()*";
        String str2_3 = "qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZX" +
                "CVBNM0x96";
        String str2_4 = "";

        test1_1(str1_1);
        test1_1(str1_2);
        test1_1(str1_3);
        test1_1(str1_4);
        test1_1(str1_5);
        test1_1(str2_1);
        test1_1(str2_2);
        test1_1(str2_3);
        test1_1(str2_4);

        StringBuilder strBuilder1_1 = new StringBuilder("abc123abc");
        StringBuilder strBuilder1_2 = new StringBuilder(" @!.&%");
        StringBuilder strBuilder1_3 = new StringBuilder("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=" +
                "!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuilder strBuilder1_4 = new StringBuilder("");
        StringBuilder strBuilder1_5 = new StringBuilder();

        test1_2(strBuilder1_1);
        test1_2(strBuilder1_2);
        test1_2(strBuilder1_3);
        test1_2(strBuilder1_4);
        test1_2(strBuilder1_5);

        // Test new StringBuilder()
        test1_3();

        // Test new StringBuilder(int capacity)
        test1_4();

        // Test new StringBuilder(CharSequence seq)
        test1_5();
    }

    private static void test1_1(String str) {
        System.out.println(new StringBuilder(str));
    }

    private static void test1_2(StringBuilder strBuilder) {
        System.out.println(strBuilder.toString());
    }

    private static void test1_3() {
        StringBuilder strBuilder1_6 = new StringBuilder();
        System.out.println(strBuilder1_6.capacity());
    }

    private static void test1_4() {
        int capacity = 20;
        StringBuilder strBuilder1_6 = new StringBuilder(capacity);
        System.out.println(strBuilder1_6.capacity());

        int capacityMin = 0;
        strBuilder1_6 = new StringBuilder(capacityMin);
        System.out.println(strBuilder1_6.capacity());
    }

    private static void test1_5() {
        CharSequence chs1_1 = "xyz";
        StringBuilder strBuilder1_7 = new StringBuilder(chs1_1);
        test1_2(strBuilder1_7);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full abc123abc\n @!.&%()*\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\n\n\nabc123ABC\n @!.&%()*\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\n\nabc123abc\n @!.&%\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\n\n\n16\n20\n0\nxyz\n0\n
