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
 * -@TestCaseID: StringBuilderExceptionsTest.java
 * -@TestCaseName: Test Exception in StringBuilder: test StringIndexOutOfBoundsException is thrown in String
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      substring(int start, int end), test StringIndexOutOfBoundsException is thrown in String
 *                      substring(int start), test IndexOutOfBoundsException is thrown in int codePointAt(int index),
 *                      test IndexOutOfBoundsException is thrown in int codePointBefore(int index), test
 *                      IndexOutOfBoundsException is thrown in int codePointCount(int beginIndex, int endIndex), test
 *                      StringIndexOutOfBoundsException is thrown in StringBuilder
 *                      replace(int start, int end, String str), test IndexOutOfBoundsException in void getChars(int
 *                      srcBegin, int srcEnd, char[] dst, int dstBegin), test IndexOutOfBoundsException in int
 *                      offsetByCodePoints(int index, int codePointOffset).
 * -@Brief:
 * -case1
 * -#step1: Create StringBuilder instance.
 * -#step2: Create Parameters：cover start/end < 0, start/end > instance.length, start > end 5 scenes.
 * -#step3: Test method substring(int start, int end), check StringIndexOutOfBoundsException is thrown.
 * -case2
 * -#step1: Create StringBuilder instance.
 * -#step2: Create Parameter: cover start < 0, start > instance.length 2 scenes.
 * -#step3: Test method substring(int start), check StringIndexOutOfBoundsException is thrown.
 * -case3
 * -#step1: Create StringBuilder instance.
 * -#step2: Create Parameter: cover index < 0, index >= instance.length 2 scenes.
 * -#step3: Test method codePointAt(int index), check IndexOutOfBoundsException is thrown.
 * -case4
 * -#step1: Create StringBuilder instance.
 * -#step2: Create Parameter: cover index < 0, index >= instance.length 2 scenes.
 * -#step3: Test method codePointBefore(int index), check IndexOutOfBoundsException is thrown.
 * -case5
 * -#step1: Create StringBuilder instance.
 * -#step2: Create Parameter: cover beginIndex/endIndex < 0, beginIndex/endIndex > instance.length, beginIndex >
 *          endIndex 5 scenes.
 * -#step3: Test method codePointCount(int beginIndex, int endIndex), check IndexOutOfBoundsException is thrown.
 * -case6
 * -#step1: Create StringBuilder instance.
 * -#step2: Create Parameter: cover start/end < 0, start/end > instance.length, start > end 5 scenes.
 * -#step3: Test method replace(int start, int end, String str),check that if StringIndexOutOfBoundsException is thrown.
 * -case7
 * -#step1: Create StringBuilder instance.
 * -#step2: Create Parameter: cover srcBegin/srcEnd < 0, srcBegin/srcEnd > instance.length, srcBegin > srcEnd, dstBegin
 *          < 0, dstBegin > dst.length, dst.length - dstBegin < srcEnd - srcBegin 8 scenes.
 * -#step3: Test method getChars(int srcBegin, int srcEnd, char[] dst, int dstBegin), check IndexOutOfBoundsException is
 *          thrown.
 * -case8
 * -#step1: Create StringBuilder instance.
 * -#step2: Create Parameter: cover index < 0, index > instance.length, codePointOffset > stringBuilder.length - index
 *          3 scenes.
 * -#step3: Test method offsetByCodePoints(int index, int codePointOffset), check IndexOutOfBoundsException is thrown.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: StringBuilderExceptionsTest.java
 * -@ExecuteClass: StringBuilderExceptionsTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringBuilderExceptionsTest {
    private static int processResult = 99;

    public static void main(String argv[]) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String argv[], PrintStream out) {
        int result = 2; /*STATUS_FAILED*/

        try {
            result = StringBuilderSubstringStringIndexOutOfBoundsException_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        try {
            result = StringBuilderSubstringStringIndexOutOfBoundsException_2();
        } catch (Exception e) {
            processResult -= 10;
        }
        try {
            result = StringBuilderCodePointAtIndexOutOfBoundsException_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        try {
            result = StringBuilderCodePointBeforeIndexOutOfBoundsException_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        try {
            result = StringBuilderCodePointCountIndexOutOfBoundsException_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        try {
            result = StringBuilderReplaceStringIndexOutOfBoundsException_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        try {
            result = StringBuilderGetCharsIndexOutOfBoundsException_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        try {
            result = StringBuilderOffsetByCodePointsIndexOutOfBoundsException_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 4 && processResult == 67) {
            result = 0;
        }
        return result;
    }

    // Test StringIndexOutOfBoundsException in String substring(int start, int end).
    public static int StringBuilderSubstringStringIndexOutOfBoundsException_1() {
        int result1 = 4; /* STATUS_FAILED */
        StringBuilder str = new StringBuilder("java is boring");

        // Test end > str.length().
        try {
            String test1 = str.substring(8, 20);
            System.out.println(test1);
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test end < 0.
        try {
            String test1 = str.substring(8, -2);
            System.out.println(test1);
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test end < start.
        try {
            String test1 = str.substring(8, 6);
            System.out.println(test1);
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test start > str.length().
        try {
            String test1 = str.substring(20, 6);
            System.out.println(test1);
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test start < 0.
        try {
            String test1 = str.substring(-2, 6);
            System.out.println(test1);
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }
        return result1;
    }

    // Test StringIndexOutOfBoundsException in String substring(int start).
    public static int StringBuilderSubstringStringIndexOutOfBoundsException_2() {
        int result1 = 4; /* STATUS_FAILED */
        StringBuilder str = new StringBuilder("java is boring");

        // Test start > str.length.
        try {
            String test1 = str.substring(20);
            System.out.println(test1);
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test start < 0.
        try {
            String test1 = str.substring(-2);
            System.out.println(test1);
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }
        return result1;
    }

    // Test IndexOutOfBoundsException in int codePointAt(int index).
    public static int StringBuilderCodePointAtIndexOutOfBoundsException_1() {
        int result1 = 4; /* STATUS_FAILED */
        StringBuilder buff = new StringBuilder("programming");

        // Test index >= stringBuilder.length.
        try {
            int str = buff.codePointAt(11);
            System.out.println(str);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test index < 0.
        try {
            int str = buff.codePointAt(-1);
            System.out.println(str);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        return result1;
    }

    // Test IndexOutOfBoundsException in int codePointBefore(int index).
    public static int StringBuilderCodePointBeforeIndexOutOfBoundsException_1() {
        int result1 = 4; /* STATUS_FAILED */
        StringBuilder buff = new StringBuilder("HelloWorld");

        // Test index > stringBuilder.length.
        try {
            int str = buff.codePointBefore(11);
            System.out.println(str);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test index < 0.
        try {
            int str = buff.codePointBefore(-3);
            System.out.println(str);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        return result1;
    }

    // Test IndexOutOfBoundsException in int codePointCount(int beginIndex, int endIndex).
    public static int StringBuilderCodePointCountIndexOutOfBoundsException_1() {
        int result1 = 4; /*STATUS_FAILED*/
        StringBuilder buff = new StringBuilder("HelloWorld");

        // Test endIndex > stringBuilder.length.
        try {
            int str = buff.codePointCount(5, 11);
            System.out.println(str);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test endIndex < beginIndex.
        try {
            int str = buff.codePointCount(5, 4);
            System.out.println(str);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test endIndex < 0.
        try {
            int str = buff.codePointCount(5, -2);
            System.out.println(str);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test beginIndex < 0.
        try {
            int str = buff.codePointCount(-1, 5);
            System.out.println(str);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test beginIndex > stringBuilder.length.
        try {
            int str = buff.codePointCount(11, 5);
            System.out.println(str);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        return result1;
    }

    // Test StringIndexOutOfBoundsException in StringBuilder replace(int start, int end, String str).
    public static int StringBuilderReplaceStringIndexOutOfBoundsException_1() {
        int result1 = 4; /* STATUS_FAILED */
        StringBuilder str = new StringBuilder("Java Util Package");

        // Test end > stringBuilder.length.
        try {
            StringBuilder test1 = str.replace(18, 20, "Lang");
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test end < start.
        try {
            StringBuilder test1 = str.replace(18, 16, "Lang");
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test end < 0.
        try {
            StringBuilder test1 = str.replace(18, -2, "Lang");
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test start < 0.
        try {
            StringBuilder test1 = str.replace(-2, 16, "Lang");
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test start > stringBuilder.length.
        try {
            StringBuilder test1 = str.replace(20, 16, "Lang");
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }
        return result1;
    }

    // Test IndexOutOfBoundsException in void getChars(int srcBegin, int srcEnd, char[] dst, int dstBegin).
    public static int StringBuilderGetCharsIndexOutOfBoundsException_1() {
        int result1 = 4; /* STATUS_FAILED */
        StringBuilder str = new StringBuilder("java programming");
        char[] cArr = new char[]{'t', 'u', 't', 'o', 'r', 'i', 'a', 'l', 's'};

        // Test cArr.length - dstBegin < srcEnd - srcBegin
        try {
            str.getChars(5, 9, cArr, 6);
            System.out.println(str);
            System.out.println(cArr);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test dstBegin > cArr.length.
        try {
            str.getChars(0, 9, cArr, 10);
            System.out.println(str);
            System.out.println(cArr);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test dstBegin < 0.
        try {
            str.getChars(0, 9, cArr, -2);
            System.out.println(str);
            System.out.println(cArr);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test srcEnd < 0.
        try {
            str.getChars(0, -2, cArr, 0);
            System.out.println(str);
            System.out.println(cArr);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test srcBegin < 0.
        try {
            str.getChars(-2, 5, cArr, 0);
            System.out.println(str);
            System.out.println(cArr);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test srcBegin > srcEnd.
        try {
            str.getChars(5, 2, cArr, 0);
            System.out.println(str);
            System.out.println(cArr);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test srcBegin > stringBuilder.length.
        try {
            str.getChars(20, 2, cArr, 0);
            System.out.println(str);
            System.out.println(cArr);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test srcEnd > stringBuilder.length.
        try {
            str.getChars(2, 20, cArr, 0);
            System.out.println(str);
            System.out.println(cArr);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        return result1;
    }

    // Test IndexOutOfBoundsException in int offsetByCodePoints(int index, int codePointOffset).
    public static int StringBuilderOffsetByCodePointsIndexOutOfBoundsException_1() {
        int result1 = 4; /* STATUS_FAILED */
        StringBuilder str = new StringBuilder("abcdefg");

        // Test codePointOffset > stringBuilder.length - index.
        try {
            int result = str.offsetByCodePoints(1, 7);
            System.out.println(str);
            System.out.println(result);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test index < 0.
        try {
            int result = str.offsetByCodePoints(-1, 3);
            System.out.println(str);
            System.out.println(result);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test index > stringBuilder.length.
        try {
            int result = str.offsetByCodePoints(8, 0);
            System.out.println(str);
            System.out.println(result);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        return result1;
    }
}



// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n