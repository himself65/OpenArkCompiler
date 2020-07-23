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
 * -@TestCaseID: StringBuilderCodePointExceptionTest.java
 * -@TestCaseName: Test method codePointAt(int index)/codePointBefore(int index)/codePointCount(int beginIndex, int
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      endIndex)/offsetByCodePoints(int index, int codePointOffset) of StringBuilder, test
 *                      StringIndexOutOfBoundsException throw in codePointAt and codePointBefore, test
 *                      IndexOutOfBoundsException throw in codePointCount and offsetByCodePoints.
 * -@Brief:
 * -#case1
 * -#step1: Create StringBuilder instance.
 * -#step2: Create parameters: index range from 0 to instance.length.
 * -#step3: Test method codePointAt(int index). Check result is correctly.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~3.
 * -#case2
 * -#step1: Create StringBuilder instance.
 * -#step2: Create parameters: index range from 0 to instance.length.
 * -#step3: Test method codePointBefore(int index). Check result is correctly, StringIndexOutOfBoundsException is thrown
 *          when index = 0.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~3.
 * -#case3
 * -#step1: Create StringBuilder instance.
 * -#step2: Create parameters: beginIndex range from 0 to instance.length - 2, endIndex = beginIndex + 3.
 * -#step3: Test method codePointCount(int beginIndex, int endIndex),Check result is correctly.IndexOutOfBoundsException
 *          is thrown when endIndex > instance.length - 1.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~3.
 * -#case4
 * -#step1: Create StringBuilder instance.
 * -#step2: Create parameters: index range from 0 to instance.length - 2, codePointOffset = index + 3.
 * -#step3: Test method int offsetByCodePoints(int index, int codePointOffset),Check result is correctly,
 *          StringIndexOutOfBoundsException is thrown when index + codePointOffset > instance.length.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~3.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringBuilderCodePointExceptionTest.java
 * -@ExecuteClass: StringBuilderCodePointExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringBuilderCodePointExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringBuilderCodePointExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringBuilderCodePointExceptionTest_1() {
        StringBuilder strBuilder1_1 = new StringBuilder("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`123456789" +
                "0-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuilder strBuilder1_2 = new StringBuilder(" @!.&%");
        StringBuilder strBuilder1_3 = new StringBuilder("abc123");

        test1(strBuilder1_1);
        test1(strBuilder1_2);
        test1(strBuilder1_3);

        test2(strBuilder1_1);
        test2(strBuilder1_2);
        test2(strBuilder1_3);

        test3(strBuilder1_1);
        test3(strBuilder1_2);
        test3(strBuilder1_3);

        test4(strBuilder1_1);
        test4(strBuilder1_2);
        test4(strBuilder1_3);
    }

    // Test method codePointAt(int index).
    private static void test1(StringBuilder strBuilder) {
        int codePoint = 0;
        for (int i = 0; i < 6; i++) {
            try {
                codePoint = strBuilder.codePointAt(i);
                System.out.println("i=" + i + " " + "codePointAt=" + codePoint);
            } catch (StringIndexOutOfBoundsException e1) {
                System.out.println("codePointAt(): " + i + "out of length");
            } finally {
                try {
                    System.out.println(strBuilder.charAt(i) + " Unicode is" + ":" + codePoint);
                } catch (StringIndexOutOfBoundsException e2) {
                    System.out.println(i + " out of length");
                }
            }
        }
    }

    // Test method codePointBefore(int index).
    private static void test2(StringBuilder strBuilder) {
        int codePoint = 0;
        for (int i = 0; i < 6; i++) {
            try {
                codePoint = strBuilder.codePointBefore(i);
                System.out.println("i=" + i + " " + "codePointBefore=" + codePoint);
            } catch (StringIndexOutOfBoundsException e1) {
                System.out.println("codePointBefore(): " + i + "out of length");
            } finally {
                try {
                    System.out.println(strBuilder.charAt(i - 1) + " Unicode is" + ":" + codePoint);
                } catch (StringIndexOutOfBoundsException e2) {
                    System.out.println(i + " out of length");
                }
            }
        }
    }

    // Test method codePointCount(int beginIndex, int endIndex).
    private static void test3(StringBuilder strBuilder) {
        int codePoint = 0;
        for (int i = 0; i < 4; i++) {
            try {
                codePoint = strBuilder.codePointCount(i, i + 3);
                System.out.println("i=" + i + " " + "codePointCount=" + codePoint);
            } catch (IndexOutOfBoundsException e1) {
                System.out.println("codePointCount(): " + i + " out of length");
            } finally {
                try {
                    System.out.println(strBuilder.charAt(i) + "~" + strBuilder.charAt(i + 3)
                            + " codePointCount is " + ":" + codePoint);
                } catch (StringIndexOutOfBoundsException e2) {
                    System.out.println(i + " out of length");
                }
            }
        }
    }

    // Test method int offsetByCodePoints(int index, int codePointOffset).
    private static void test4(StringBuilder strBuilder) {
        int codePoint = 0;
        for (int i = 0; i < 4; i++) {
            try {
                codePoint = strBuilder.offsetByCodePoints(i, i + 3);
                System.out.println("i=" + i + " " + "offsetByCodePoints=" + codePoint);
            } catch (IndexOutOfBoundsException e1) {
                System.out.println("offsetByCodePoints(): " + i + "+3  out of length");
            } finally {
                try {
                    System.out.println(strBuilder.charAt(i) + " offsetByCodePoints +3 is " + ":" + codePoint);
                } catch (StringIndexOutOfBoundsException e2) {
                    System.out.println("charAt(): " + i + " out of length");
                }
            }
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full i=0 codePointAt=113\nq Unicode is:113\ni=1 codePointAt=119\nw Unicode is:119\ni=2 codePointAt=101\ne Unicode is:101\ni=3 codePointAt=114\nr Unicode is:114\ni=4 codePointAt=116\nt Unicode is:116\ni=5 codePointAt=121\ny Unicode is:121\ni=0 codePointAt=32\n  Unicode is:32\ni=1 codePointAt=64\n@ Unicode is:64\ni=2 codePointAt=33\n! Unicode is:33\ni=3 codePointAt=46\n. Unicode is:46\ni=4 codePointAt=38\n& Unicode is:38\ni=5 codePointAt=37\n% Unicode is:37\ni=0 codePointAt=97\na Unicode is:97\ni=1 codePointAt=98\nb Unicode is:98\ni=2 codePointAt=99\nc Unicode is:99\ni=3 codePointAt=49\n1 Unicode is:49\ni=4 codePointAt=50\n2 Unicode is:50\ni=5 codePointAt=51\n3 Unicode is:51\ncodePointBefore(): 0out of length\n0 out of length\ni=1 codePointBefore=113\nq Unicode is:113\ni=2 codePointBefore=119\nw Unicode is:119\ni=3 codePointBefore=101\ne Unicode is:101\ni=4 codePointBefore=114\nr Unicode is:114\ni=5 codePointBefore=116\nt Unicode is:116\ncodePointBefore(): 0out of length\n0 out of length\ni=1 codePointBefore=32\n  Unicode is:32\ni=2 codePointBefore=64\n@ Unicode is:64\ni=3 codePointBefore=33\n! Unicode is:33\ni=4 codePointBefore=46\n. Unicode is:46\ni=5 codePointBefore=38\n& Unicode is:38\ncodePointBefore(): 0out of length\n0 out of length\ni=1 codePointBefore=97\na Unicode is:97\ni=2 codePointBefore=98\nb Unicode is:98\ni=3 codePointBefore=99\nc Unicode is:99\ni=4 codePointBefore=49\n1 Unicode is:49\ni=5 codePointBefore=50\n2 Unicode is:50\ni=0 codePointCount=3\nq~r codePointCount is :3\ni=1 codePointCount=3\nw~t codePointCount is :3\ni=2 codePointCount=3\ne~y codePointCount is :3\ni=3 codePointCount=3\nr~u codePointCount is :3\ni=0 codePointCount=3\n ~. codePointCount is :3\ni=1 codePointCount=3\n@~& codePointCount is :3\ni=2 codePointCount=3\n!~% codePointCount is :3\ni=3 codePointCount=3\n3 out of length\ni=0 codePointCount=3\na~1 codePointCount is :3\ni=1 codePointCount=3\nb~2 codePointCount is :3\ni=2 codePointCount=3\nc~3 codePointCount is :3\ni=3 codePointCount=3\n3 out of length\ni=0 offsetByCodePoints=3\nq offsetByCodePoints +3 is :3\ni=1 offsetByCodePoints=5\nw offsetByCodePoints +3 is :5\ni=2 offsetByCodePoints=7\ne offsetByCodePoints +3 is :7\ni=3 offsetByCodePoints=9\nr offsetByCodePoints +3 is :9\ni=0 offsetByCodePoints=3\n  offsetByCodePoints +3 is :3\ni=1 offsetByCodePoints=5\n@ offsetByCodePoints +3 is :5\noffsetByCodePoints(): 2+3  out of length\n! offsetByCodePoints +3 is :5\noffsetByCodePoints(): 3+3  out of length\n. offsetByCodePoints +3 is :5\ni=0 offsetByCodePoints=3\na offsetByCodePoints +3 is :3\ni=1 offsetByCodePoints=5\nb offsetByCodePoints +3 is :5\noffsetByCodePoints(): 2+3  out of length\nc offsetByCodePoints +3 is :5\noffsetByCodePoints(): 3+3  out of length\n1 offsetByCodePoints +3 is :5\n0\n
