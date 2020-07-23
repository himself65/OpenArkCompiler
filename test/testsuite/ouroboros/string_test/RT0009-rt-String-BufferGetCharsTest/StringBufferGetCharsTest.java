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
 * -@TestCaseID: StringBufferGetCharsTest.java
 * -@TestCaseName: Test Method getChars(int srcBegin, int srcEnd, char[] dst, int dstBegin) of StringBuffer.
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create StringBuffer instance.
 * -#step2: Create parameters: srcBegin  =< srcEnd < instance.length,, dstBegin < dst(char[]).length, srcBegin/srcEnd
 *          /dstBegin coverage boundary value.
 * -#step3: Test Method void getChars(int srcBegin, int srcEnd, char[] dst, int dstBegin).
 * -#step4: Check some chars of dst(char[]) is replaced correctly.
 * -#step5: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~4.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringBufferGetCharsTest.java
 * -@ExecuteClass: StringBufferGetCharsTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringBufferGetCharsTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringBufferGetCharsTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringBufferGetCharsTest_1() {
        StringBuffer strBuffer1_1 = new StringBuffer("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuffer strBuffer1_2 = new StringBuffer(" @!.&%()*");
        StringBuffer strBuffer1_3 = new StringBuffer("abc123");

        test1(strBuffer1_1);
        test1(strBuffer1_2);
        test1(strBuffer1_3);
    }

    private static void test1(StringBuffer strBuffer) {
        // Test srcBegin  < srcEnd < instance.length, dstBegin < dst(char[]).length.
        char[] dst = {'A', 'B', 'C', 'D', 'E', 'F'};
        strBuffer.getChars(2, 5, dst, 2);
        System.out.println(dst);

        // Test srcBegin = 0.
        dst = new char[]{'A', 'B', 'C', 'D', 'E', 'F'};
        strBuffer.getChars(0, 3, dst, 2);
        System.out.println(dst);

        // Test srcEnd = 0.
        dst = new char[]{'A', 'B', 'C', 'D', 'E', 'F'};
        strBuffer.getChars(0, 0, dst, 2);
        System.out.println(dst);

        // Test dstBegin = 0.
        dst = new char[]{'A', 'B', 'C', 'D', 'E', 'F'};
        strBuffer.getChars(2, 5, dst, 0);
        System.out.println(dst);

        // Test srcBegin = srcEnd.
        dst = new char[]{'A', 'B', 'C', 'D', 'E', 'F'};
        strBuffer.getChars(2, 2, dst, 0);
        System.out.println(dst);

        // Test srcBegin = strBuffer.length() - 1.
        dst = new char[]{'A', 'B', 'C', 'D', 'E', 'F'};
        int index = strBuffer.length() - 1;
        strBuffer.getChars(index, index, dst, 0);
        System.out.println(dst);

        // Test srcEnd = strBuffer.length() - 1.
        dst = new char[]{'A', 'B', 'C', 'D', 'E', 'F'};
        strBuffer.getChars(index - 2, index, dst, 0);
        System.out.println(dst);

        // Test dstBegin = dst.length - 1.
        dst = new char[]{'A', 'B', 'C', 'D', 'E', 'F'};
        strBuffer.getChars(0, 1, dst, dst.length - 1);
        System.out.println(dst);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ABertF\nABqweF\nABCDEF\nertDEF\nABCDEF\nABCDEF\nx9CDEF\nABCDEq\nAB!.&F\nAB @!F\nABCDEF\n!.&DEF\nABCDEF\nABCDEF\n()CDEF\nABCDE \nABc12F\nABabcF\nABCDEF\nc12DEF\nABCDEF\nABCDEF\n12CDEF\nABCDEa\n0\n

