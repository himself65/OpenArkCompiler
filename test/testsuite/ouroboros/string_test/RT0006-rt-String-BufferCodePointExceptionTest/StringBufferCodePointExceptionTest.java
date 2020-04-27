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
 * -@TestCaseID: StringBufferCodePointExceptionTest.java
 * -@TestCaseName: Test method codePointAt(int index)/codePointBefore(int index)/codePointCount(int beginIndex, int
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      endIndex)/offsetByCodePoints(int index, int codePointOffset) of StringBuffer, test
 *                      StringIndexOutOfBoundsException throw in codePointAt and codePointBefore, test
 *                      IndexOutOfBoundsException throw in codePointCount and offsetByCodePoints.
 * -@Brief:
 * -#case1
 * -#step1: Create StringBuffer instance.
 * -#step2: Create parameters: index range from 0 to instance.length.
 * -#step3: Test method codePointAt(int index). Check result is correctly.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~3.
 * -#case2
 * -#step1: Create StringBuffer instance.
 * -#step2: Create parameters: index range from 0 to instance.length.
 * -#step3: Test method codePointBefore(int index). Check result is correctly, StringIndexOutOfBoundsException is thrown
 *          when index = 0.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~3.
 * -#case3
 * -#step1: Create StringBuffer instance.
 * -#step2: Create parameters: beginIndex range from 0 to instance.length - 2, endIndex = beginIndex + 3.
 * -#step3: Test method codePointCount(int beginIndex, int endIndex),Check result is correctly.IndexOutOfBoundsException
 *          is thrown when endIndex > instance.length - 1.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~3.
 * -#case4
 * -#step1: Create StringBuffer instance.
 * -#step2: Create parameters: index range from 0 to instance.length - 2, codePointOffset = index + 3.
 * -#step3: Test method int offsetByCodePoints(int index, int codePointOffset),Check result is correctly,
 *          StringIndexOutOfBoundsException is thrown when index + codePointOffset > instance.length.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~3.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringBufferCodePointExceptionTest.java
 * -@ExecuteClass: StringBufferCodePointExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringBufferCodePointExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringBufferCodePointExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringBufferCodePointExceptionTest_1() {
        StringBuffer strBuffer1_1 = new StringBuffer("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-" +
                "=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuffer strBuffer1_2 = new StringBuffer(" @!.&%");
        StringBuffer strBuffer1_3 = new StringBuffer("abc123");

        test1(strBuffer1_1);
        test1(strBuffer1_2);
        test1(strBuffer1_3);

        test2(strBuffer1_1);
        test2(strBuffer1_2);
        test2(strBuffer1_3);

        test3(strBuffer1_1);
        test3(strBuffer1_2);
        test3(strBuffer1_3);

        test4(strBuffer1_1);
        test4(strBuffer1_2);
        test4(strBuffer1_3);
    }

    // Test method codePointAt(int index).
    private static void test1(StringBuffer strBuffer) {
        int codePoint = 0;
        for (int i = 0; i < 6; i++) {
            try {
                codePoint = strBuffer.codePointAt(i);
                System.out.println("i=" + i + " " + "codePointAt=" + codePoint);
            } catch (StringIndexOutOfBoundsException e1) {
                System.out.println("codePointAt(): " + i + "out of length");
            } finally {
                try {
                    System.out.println(strBuffer.charAt(i) + " Unicode is" + ":" + codePoint);
                } catch (StringIndexOutOfBoundsException e2) {
                    System.out.println(i + " out of length");
                }
            }
        }
    }

    // Test method codePointBefore(int index).
    private static void test2(StringBuffer strBuffer) {
        int codePoint = 0;
        for (int i = 0; i < 6; i++) {
            try {
                codePoint = strBuffer.codePointBefore(i);
                System.out.println("i=" + i + " " + "codePointBefore=" + codePoint);
            } catch (StringIndexOutOfBoundsException e1) {
                System.out.println("codePointBefore(): " + i + "out of length");
            } finally {
                try {
                    System.out.println(strBuffer.charAt(i - 1) + " Unicode is" + ":" + codePoint);
                } catch (StringIndexOutOfBoundsException e2) {
                    System.out.println(i + " out of length");
                }
            }
        }
    }

    // Test method codePointCount(int beginIndex, int endIndex).
    private static void test3(StringBuffer strBuffer) {
        int codePoint = 0;
        for (int i = 0; i < 4; i++) {
            try {
                codePoint = strBuffer.codePointCount(i, i + 3);
                System.out.println("i=" + i + " " + "codePointCount=" + codePoint);
            } catch (IndexOutOfBoundsException e1) {
                System.out.println("codePointCount(): " + i + " out of length");
            } finally {
                try {
                    System.out.println(strBuffer.charAt(i) + "~" + strBuffer.charAt(i + 3) +
                            " codePointCount is " + ":" + codePoint);
                } catch (StringIndexOutOfBoundsException e2) {
                    System.out.println(i + " out of length");
                }
            }
        }
    }

    // Test method int offsetByCodePoints(int index, int codePointOffset).
    private static void test4(StringBuffer strBuffer) {
        int codePoint = 0;
        for (int i = 0; i < 4; i++) {
            try {
                codePoint = strBuffer.offsetByCodePoints(i, i + 3);
                System.out.println("i=" + i + " " + "offsetByCodePoints=" + codePoint);
            } catch (IndexOutOfBoundsException e1) {
                System.out.println("offsetByCodePoints(): " + i + "+3  out of length");
            } finally {
                try {
                    System.out.println(strBuffer.charAt(i) + " offsetByCodePoints +3 is " + ":" + codePoint);
                } catch (StringIndexOutOfBoundsException e2) {
                    System.out.println("charAt(): " + i + " out of length");
                }
            }
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan i\=0\s*codePointAt\=113\s*q\s*Unicode\s*is\:113\s*i\=1\s*codePointAt\=119\s*w\s*Unicode\s*is\:119\s*i\=2\s*codePointAt\=101\s*e\s*Unicode\s*is\:101\s*i\=3\s*codePointAt\=114\s*r\s*Unicode\s*is\:114\s*i\=4\s*codePointAt\=116\s*t\s*Unicode\s*is\:116\s*i\=5\s*codePointAt\=121\s*y\s*Unicode\s*is\:121\s*i\=0\s*codePointAt\=32\s*Unicode\s*is\:32\s*i\=1\s*codePointAt\=64\s*\@\s*Unicode\s*is\:64\s*i\=2\s*codePointAt\=33\s*\!\s*Unicode\s*is\:33\s*i\=3\s*codePointAt\=46\s*\.\s*Unicode\s*is\:46\s*i\=4\s*codePointAt\=38\s*\&\s*Unicode\s*is\:38\s*i\=5\s*codePointAt\=37\s*\%\s*Unicode\s*is\:37\s*i\=0\s*codePointAt\=97\s*a\s*Unicode\s*is\:97\s*i\=1\s*codePointAt\=98\s*b\s*Unicode\s*is\:98\s*i\=2\s*codePointAt\=99\s*c\s*Unicode\s*is\:99\s*i\=3\s*codePointAt\=49\s*1\s*Unicode\s*is\:49\s*i\=4\s*codePointAt\=50\s*2\s*Unicode\s*is\:50\s*i\=5\s*codePointAt\=51\s*3\s*Unicode\s*is\:51\s*codePointBefore\(\)\:\s*0out\s*of\s*length\s*0\s*out\s*of\s*length\s*i\=1\s*codePointBefore\=113\s*q\s*Unicode\s*is\:113\s*i\=2\s*codePointBefore\=119\s*w\s*Unicode\s*is\:119\s*i\=3\s*codePointBefore\=101\s*e\s*Unicode\s*is\:101\s*i\=4\s*codePointBefore\=114\s*r\s*Unicode\s*is\:114\s*i\=5\s*codePointBefore\=116\s*t\s*Unicode\s*is\:116\s*codePointBefore\(\)\:\s*0out\s*of\s*length\s*0\s*out\s*of\s*length\s*i\=1\s*codePointBefore\=32\s*Unicode\s*is\:32\s*i\=2\s*codePointBefore\=64\s*\@\s*Unicode\s*is\:64\s*i\=3\s*codePointBefore\=33\s*\!\s*Unicode\s*is\:33\s*i\=4\s*codePointBefore\=46\s*\.\s*Unicode\s*is\:46\s*i\=5\s*codePointBefore\=38\s*\&\s*Unicode\s*is\:38\s*codePointBefore\(\)\:\s*0out\s*of\s*length\s*0\s*out\s*of\s*length\s*i\=1\s*codePointBefore\=97\s*a\s*Unicode\s*is\:97\s*i\=2\s*codePointBefore\=98\s*b\s*Unicode\s*is\:98\s*i\=3\s*codePointBefore\=99\s*c\s*Unicode\s*is\:99\s*i\=4\s*codePointBefore\=49\s*1\s*Unicode\s*is\:49\s*i\=5\s*codePointBefore\=50\s*2\s*Unicode\s*is\:50\s*i\=0\s*codePointCount\=3\s*q\~r\s*codePointCount\s*is\s*\:3\s*i\=1\s*codePointCount\=3\s*w\~t\s*codePointCount\s*is\s*\:3\s*i\=2\s*codePointCount\=3\s*e\~y\s*codePointCount\s*is\s*\:3\s*i\=3\s*codePointCount\=3\s*r\~u\s*codePointCount\s*is\s*\:3\s*i\=0\s*codePointCount\=3\s*\~\.\s*codePointCount\s*is\s*\:3\s*i\=1\s*codePointCount\=3\s*\@\~\&\s*codePointCount\s*is\s*\:3\s*i\=2\s*codePointCount\=3\s*\!\~\%\s*codePointCount\s*is\s*\:3\s*i\=3\s*codePointCount\=3\s*3\s*out\s*of\s*length\s*i\=0\s*codePointCount\=3\s*a\~1\s*codePointCount\s*is\s*\:3\s*i\=1\s*codePointCount\=3\s*b\~2\s*codePointCount\s*is\s*\:3\s*i\=2\s*codePointCount\=3\s*c\~3\s*codePointCount\s*is\s*\:3\s*i\=3\s*codePointCount\=3\s*3\s*out\s*of\s*length\s*i\=0\s*offsetByCodePoints\=3\s*q\s*offsetByCodePoints\s*\+3\s*is\s*\:3\s*i\=1\s*offsetByCodePoints\=5\s*w\s*offsetByCodePoints\s*\+3\s*is\s*\:5\s*i\=2\s*offsetByCodePoints\=7\s*e\s*offsetByCodePoints\s*\+3\s*is\s*\:7\s*i\=3\s*offsetByCodePoints\=9\s*r\s*offsetByCodePoints\s*\+3\s*is\s*\:9\s*i\=0\s*offsetByCodePoints\=3\s*offsetByCodePoints\s*\+3\s*is\s*\:3\s*i\=1\s*offsetByCodePoints\=5\s*\@\s*offsetByCodePoints\s*\+3\s*is\s*\:5\s*offsetByCodePoints\(\)\:\s*2\+3\s*out\s*of\s*length\s*\!\s*offsetByCodePoints\s*\+3\s*is\s*\:5\s*offsetByCodePoints\(\)\:\s*3\+3\s*out\s*of\s*length\s*\.\s*offsetByCodePoints\s*\+3\s*is\s*\:5\s*i\=0\s*offsetByCodePoints\=3\s*a\s*offsetByCodePoints\s*\+3\s*is\s*\:3\s*i\=1\s*offsetByCodePoints\=5\s*b\s*offsetByCodePoints\s*\+3\s*is\s*\:5\s*offsetByCodePoints\(\)\:\s*2\+3\s*out\s*of\s*length\s*c\s*offsetByCodePoints\s*\+3\s*is\s*\:5\s*offsetByCodePoints\(\)\:\s*3\+3\s*out\s*of\s*length\s*1\s*offsetByCodePoints\s*\+3\s*is\s*\:5\s*0