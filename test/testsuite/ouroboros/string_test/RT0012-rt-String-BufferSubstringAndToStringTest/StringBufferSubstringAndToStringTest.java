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
 * -@TestCaseID: StringBufferSubstringAndToStringTest.java
 * -@TestCaseName: Test Method:String substring(int start)/String substring(int start, int end)/String toString()
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
  *                     of StringBuffer.
 * -@Brief:
 * -#case1
 * -#step1: Create StringBuffer instance.
 * -#step2: Create Parameters: start < instance.length, start < end < instance.length, start and end coverage boundary
 *          value.
 * -#step3: Test method substring(int start, int end).
 * -#step4: Check the return result of substring is correctly.
 * -#step5: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~4.
 * -#case2
 * -#step1: Create StringBuffer instance.
 * -#step2: Create Parameters: 0 =< start < instance.length, start coverage boundary value.
 * -#step3: Test method substring(int start).
 * -#step4: Check the return result of substring is correctly.
 * -#step5: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~4.
 * -#case3
 * -#step1: Create StringBuffer instance.
 * -#step2: Test method toString().
 * -#step3: Check the return result of toString is correctly.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols/""/null to repeat step2-3.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringBufferSubstringAndToStringTest.java
 * -@ExecuteClass: StringBufferSubstringAndToStringTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringBufferSubstringAndToStringTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringBufferSubstringAndToStringTest_1();
            StringBufferSubstringAndToStringTest_2();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringBufferSubstringAndToStringTest_1() {
        StringBuffer strBuffer1_1 = new StringBuffer("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuffer strBuffer1_2 = new StringBuffer(" @!.&%()*");
        StringBuffer strBuffer1_3 = new StringBuffer("abc123abc");

        test1(strBuffer1_1);
        test1(strBuffer1_2);
        test1(strBuffer1_3);

        test2(strBuffer1_1);
        test2(strBuffer1_2);
        test2(strBuffer1_3);
    }

    public static void StringBufferSubstringAndToStringTest_2() {
        StringBuffer strBuffer1_1 = new StringBuffer("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuffer strBuffer1_2 = new StringBuffer(" @!.&%");
        StringBuffer strBuffer1_3 = new StringBuffer("abc123abc");
        StringBuffer strBuffer1_4 = new StringBuffer("");
        StringBuffer strBuffer1_5 = new StringBuffer();

        test3(strBuffer1_1);
        test3(strBuffer1_2);
        test3(strBuffer1_3);
        test3(strBuffer1_4);
        test3(strBuffer1_5);
    }

    // Test method substring(int start, int end).
    private static void test1(StringBuffer strBuffer) {
        System.out.println(strBuffer.substring(2, 7));
        // Test start = 0 & end = strBuffer.length().
        System.out.println(strBuffer.substring(0, strBuffer.length()));
        // Test start = strBuffer.length().
        System.out.println(strBuffer.substring(strBuffer.length(), strBuffer.length()));
        // Test end = 0.
        System.out.println(strBuffer.substring(0, 0));
    }

    // Test method substring(int start).
    private static void test2(StringBuffer strBuffer) {
        System.out.println(strBuffer.substring(3));
        // Test start = 0.
        System.out.println(strBuffer.substring(0));
        // Test start = strBuffer.length().
        System.out.println(strBuffer.substring(strBuffer.length()));
    }

    // Test method toString().
    private static void test3(StringBuffer strBuffer) {
        System.out.println(strBuffer.toString());
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ertyu\s*qwertyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*\!\.\&\%\(\s*\@\!\.\&\%\(\)\*\s*c123a\s*abc123abc\s*rtyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*qwertyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*\.\&\%\(\)\*\s*\@\!\.\&\%\(\)\*\s*123abc\s*abc123abc\s*qwertyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*\@\!\.\&\%\s*abc123abc\s*0