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
 * -@TestCaseID: StringBufferExceptionsTest.java
 * -@TestCaseName: Test Exception in StringBuffer: test IndexOutOfBoundsException is thrown in public void
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      getChars(int srcBegin, int srcEnd, char[] dst, int dstBegin), test
 *                      StringIndexOutOfBoundsException is thrown in public StringBuffer replace(int start, int end,
 *                      String str), test StringIndexOutOfBoundsException is thrown in public String
 *                      substring(int start, int end), test StringIndexOutOfBoundsException is thrown in public String
 *                      substring(int start).
 * -@Brief:
 * -case1
 * -#step1: Create StringBuffer instance.
 * -#step2: Create Parameters：cover srcBegin/srcEnd < 0, srcBegin/srcEnd > instance.length, srcBegin > srcEnd, dstBegin
 *          < 0, dstBegin > dst.length, dst.length - dstBegin < srcEnd - srcBegin 8 scenes.
 * -#step3: Test method getChars(int srcBegin, int srcEnd, char[] dst, int dstBegin), check IndexOutOfBoundsException is
 *          thrown.
 * -case2
 * -#step1: Create StringBuffer instance.
 * -#step2: Create Parameters：cover start/end < 0, start > instance.length, start > end 4 scenes.
 * -#step3: Test method replace(int start, int end, String str), check StringIndexOutOfBoundsException is thrown.
 * -case3
 * -#step1: Create StringBuffer instance.
 * -#step2: Create Parameters：cover start/end < 0, start/end > instance.length, start > end 5 scenes.
 * -#step3: Test method substring(int start, int end), check StringIndexOutOfBoundsException is thrown.
 * -case4
 * -#step1: Create StringBuffer instance.
 * -#step2: Create Parameters：cover start < 0, start > instance.length 2 scenes.
 * -#step3: Test method substring(int start), check StringIndexOutOfBoundsException is thrown.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: StringBufferExceptionsTest.java
 * -@ExecuteClass: StringBufferExceptionsTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringBufferExceptionsTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2;  /* STATUS_Success */

        try {
            result = StringBufferGetCharsIndexOutOfBoundsException_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        try {
            result = StringBufferReplaceStringIndexOutOfBoundsException_2();
        } catch (Exception e) {
            processResult -= 10;
        }
        try {
            result = StringBufferSubstringStringIndexOutOfBoundsException_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        try {
            result = StringBufferSubstringStringIndexOutOfBoundsException_2();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 80) {
            result = 0;
        }
        return result;
    }

    public static int StringBufferGetCharsIndexOutOfBoundsException_1() {
        int result1 = 3; /*STATUS_FAILED*/

        // IndexOutOfBoundsException- If either off or len is negative, or if off + len is greater than b.length.
        // Test public void getChars(int srcBegin, int srcEnd, char[] dst, int dstBegin).
        StringBuffer buff = new StringBuffer("java programming");
        char[] chArr = new char[]{'t', 'u', 't', 'o', 'r', 'i', 'a', 'l', 's'};

        // Test dstBegin > dst.length.
        try {
            buff.getChars(5, 10, chArr, 30);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test dstBegin < 0.
        try {
            buff.getChars(5, 10, chArr, -2);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test dst.length - dstBegin < srcEnd - srcBegin.
        try {
            buff.getChars(5, 10, chArr, 7);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test srcBegin < 0.
        try {
            buff.getChars(-2, 10, chArr, 5);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test srcBegin > buff.length.
        try {
            buff.getChars(buff.length(), 10, chArr, 5);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test srcEnd < 0.
        try {
            buff.getChars(5, -2, chArr, 5);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test srcEnd < srcBegin.
        try {
            buff.getChars(5, 2, chArr, 5);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test srcEnd > buff.length.
        try {
            buff.getChars(5, buff.length(), chArr, 5);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        return result1;
    }

    public static int StringBufferReplaceStringIndexOutOfBoundsException_2() {
        int result1 = 3; /*STATUS_FAILED*/

        // StringIndexOutOfBoundsException- Access parameter range of index.
        // Test public StringBuffer replace(int start, int end, String str).
        StringBuffer buff1 = new StringBuffer("HelloWorld");

        // Test start > buff1.length.
        try {
            buff1.replace(20, 6, "QQQ");
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test start < 0.
        try {
            buff1.replace(-2, 6, "QQQ");
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test start > end.
        try {
            buff1.replace(7, 6, "QQQ");
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test end < 0.
        try {
            buff1.replace(7, -2, "QQQ");
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }
        return result1;
    }

    public static int StringBufferSubstringStringIndexOutOfBoundsException_1() {
        int result1 = 3; /*STATUS_FAILED*/

        // StringIndexOutOfBoundsException- Intercept character string specifying scope, exceed the range error.
        // Test public String substring(int start, int end).
        StringBuffer buff2 = new StringBuffer("HelloWorld");

        // Test end > buff2.length.
        try {
            String str = buff2.substring(9, 11);
            System.out.println(str);
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test end < 0.
        try {
            String str = buff2.substring(9, 11);
            System.out.println(str);
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test start > buff2.length.
        try {
            String str = buff2.substring(11, 5);
            System.out.println(str);
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test start > end.
        try {
            String str = buff2.substring(8, 5);
            System.out.println(str);
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test start < 0.
        try {
            String str = buff2.substring(-2, 5);
            System.out.println(str);
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }
        return result1;
    }

    public static int StringBufferSubstringStringIndexOutOfBoundsException_2() {
        int result1 = 3; /*STATUS_FAILED*/

        // StringIndexOutOfBoundsException- Intercept character string specifying scope, exceed the range error.
        // Test public String substring(int start).
        StringBuffer buff3 = new StringBuffer("HelloWorld");

        // Test start > buff3.length.
        try {
            String str = buff3.substring(15);
            System.out.println(str);
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }

        // Test start < 0.
        try {
            String str = buff3.substring(-2);
            System.out.println(str);
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            result1 = 2;
            processResult--;
        }
        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n