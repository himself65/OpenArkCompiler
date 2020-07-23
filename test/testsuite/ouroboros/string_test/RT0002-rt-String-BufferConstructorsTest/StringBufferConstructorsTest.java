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
 * -@TestCaseID: StringBufferConstructorsTest.java
 * -@TestCaseName: Test String Buffer Constructors StringBuffer(String str)/StringBuffer()/StringBuffer(int capacity)
 *                     /StringBuffer(CharSequence seq).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#case1
 * -#step1: Create a String instance By new and not new.
 * -#step2: Test Constructor StringBuffer(String str).
 * -#step3: Check new StringBuffer instance is new correctly.
 * -#step4: Change Parameter str Traversing String mix with Letter and special symbols and Number、empty String、null to
 *          repeat step2~3.
 * -#case2
 * -#step1: Test new StringBuffer() and check the capacity() of instance is correctly.
 * -#step2: Change instance as One or more kinds of letters, numbers, special symbols/""/null to repeat step1.
 * -#case3
 * -#step1: Create capacity > 0 or capacity = 0, test new StringBuffer(int capacity), check the capacity() of instance
 *          is correctly.
 * -#case4
 * -#step1: Create seq Contains numbers, letters, special symbols, test StringBuffer(CharSequence seq), check new
 *          instance is correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringBufferConstructorsTest.java
 * -@ExecuteClass: StringBufferConstructorsTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringBufferConstructorsTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringBufferConstructorsTest_1();
        } catch (Exception e) {
            e.printStackTrace();
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringBufferConstructorsTest_1() {
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

        StringBuffer strBuffer1_1 = new StringBuffer("abc123abc");
        StringBuffer strBuffer1_2 = new StringBuffer(" @!.&%()*");
        StringBuffer strBuffer1_3 = new StringBuffer("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=" +
                                    "!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuffer strBuffer1_4 = new StringBuffer("");
        StringBuffer strBuffer1_5 = new StringBuffer();

        test1_2(strBuffer1_1);
        test1_2(strBuffer1_2);
        test1_2(strBuffer1_3);
        test1_2(strBuffer1_4);
        test1_2(strBuffer1_5);

        test1_3();
        test1_4();
        test1_5();
    }

    private static void test1_1(String str) {
        System.out.println(new StringBuffer(str));
    }

    private static void test1_2(StringBuffer strBuffer) {
        System.out.println(strBuffer.toString());
    }

    // Test new StringBuffer().
    private static void test1_3() {
        StringBuffer strBuffer1_6 = new StringBuffer();
        System.out.println(strBuffer1_6.capacity());
    }

    // Test new StringBuffer(int capacity).
    private static void test1_4() {
        int capacity = 20;
        StringBuffer strBuffer1_6 = new StringBuffer(capacity);
        System.out.println(strBuffer1_6.capacity());
        int capacityMin = 0;
        strBuffer1_6 = new StringBuffer(capacityMin);
        System.out.println(strBuffer1_6.capacity());
    }

    // Test new StringBuffer(CharSequence seq).
    private static void test1_5() {
        CharSequence chs1_1 = "qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTY" +
                "UIOPZXCVBNM0x96";
        StringBuffer strBuffer1_7 = new StringBuffer(chs1_1);
        test1_2(strBuffer1_7);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full abc123abc\n @!.&%()*\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\n\n\nabc123ABC\n @!.&%()*\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\n\nabc123abc\n @!.&%()*\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\n\n\n16\n20\n0\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\n0\n
