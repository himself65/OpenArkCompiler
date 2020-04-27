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
 * -@TestCaseID: StringBuilderGetCharsTest.java
 * -@TestCaseName: Test Method getChars(int srcBegin, int srcEnd, char[] dst, int dstBegin) of StringBuilder.
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create StringBuilder instance by new StringBuilder(String stringLine).
 * -#step2: Create parameters: instance.length  > srcEnd > srcBegin,dstBegin < dst(char[]).length, srcBegin/srcEnd
 *          /dstBegin coverage boundary value.
 * -#step3: Test Method void getChars(int srcBegin, int srcEnd, char[] dst, int dstBegin).
 * -#step4: Check some chars of dst(char[]) is replaced correctly.
 * -#step5: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~4.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringBuilderGetCharsTest.java
 * -@ExecuteClass: StringBuilderGetCharsTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringBuilderGetCharsTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringBuilderGetCharsTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringBuilderGetCharsTest_1() {
        StringBuilder strBuilder1_1 = new StringBuilder("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890" +
                "-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuilder strBuilder1_2 = new StringBuilder(" @!.&%()*");
        StringBuilder strBuilder1_3 = new StringBuilder("abc123");

        test1(strBuilder1_1);
        test1(strBuilder1_2);
        test1(strBuilder1_3);
    }

    private static void test1(StringBuilder strBuilder) {
        // Test srcBegin  < srcEnd < instance.length, dstBegin < dst(char[]).length.
        char[] dst = {'A', 'B', 'C', 'D', 'E', 'F'};
        strBuilder.getChars(2, 5, dst, 2);
        System.out.println(dst);

        // Test srcBegin = 0.
        dst = new char[]{'A', 'B', 'C', 'D', 'E', 'F'};
        strBuilder.getChars(0, 3, dst, 2);
        System.out.println(dst);

        // Test srcEnd = 0.
        dst = new char[]{'A', 'B', 'C', 'D', 'E', 'F'};
        strBuilder.getChars(0, 0, dst, 2);
        System.out.println(dst);

        // Test dstBegin = 0.
        dst = new char[]{'A', 'B', 'C', 'D', 'E', 'F'};
        strBuilder.getChars(2, 5, dst, 0);
        System.out.println(dst);

        // Test strBuilder = srcEnd.
        dst = new char[]{'A', 'B', 'C', 'D', 'E', 'F'};
        strBuilder.getChars(2, 2, dst, 0);
        System.out.println(dst);

        // Test srcBegin = strBuffer.length() - 1.
        dst = new char[]{'A', 'B', 'C', 'D', 'E', 'F'};
        int index = strBuilder.length() - 1;
        strBuilder.getChars(index, index, dst, 0);
        System.out.println(dst);

        // Test srcEnd = strBuffer.length() - 1.
        dst = new char[]{'A', 'B', 'C', 'D', 'E', 'F'};
        strBuilder.getChars(index - 2, index, dst, 0);
        System.out.println(dst);

        // Test dstBegin = dst.length-1.
        dst = new char[]{'A', 'B', 'C', 'D', 'E', 'F'};
        strBuilder.getChars(0, 1, dst, dst.length - 1);
        System.out.println(dst);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ABertF\s*ABqweF\s*ABCDEF\s*ertDEF\s*ABCDEF\s*ABCDEF\s*x9CDEF\s*ABCDEq\s*AB\!\.\&F\s*AB\s*\@\!F\s*ABCDEF\s*\!\.\&DEF\s*ABCDEF\s*ABCDEF\s*\(\)CDEF\s*ABCDE\s*ABc12F\s*ABabcF\s*ABCDEF\s*c12DEF\s*ABCDEF\s*ABCDEF\s*12CDEF\s*ABCDEa\s*0