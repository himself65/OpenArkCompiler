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
 * -@TestCaseID: StringBufferDeleteAndDeleteCharAtTest.java
 * -@TestCaseName: Test Method StringBuffer delete(int start, int end) and StringBuffer deleteCharAt(int index) of
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      StringBuffer.
 * -@Brief:
 * -#case1
 * -#step1: Create StringBuffer instance.
 * -#step2: Create parameters: 0 =< start =< end, end =< instance.length, start and end coverage boundary value.
 * -#step3: Test Method delete(int start, int end) and check result is correctly.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~3.
 * -#case2
 * -#step1: Create StringBuffer instance.
 * -#step2: Create parameters: 0 =< index < instance.length, index coverage boundary value.
 * -#step3: Test Method deleteCharAt(int index) and check result is correctly.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~3.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringBufferDeleteAndDeleteCharAtTest.java
 * -@ExecuteClass: StringBufferDeleteAndDeleteCharAtTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringBufferDeleteAndDeleteCharAtTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringBufferDeleteAndDeleteCharAtTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringBufferDeleteAndDeleteCharAtTest_1() {
        StringBuffer strBuffer1_1 = new StringBuffer("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuffer strBuffer1_2 = new StringBuffer(" @!.&%()*");
        StringBuffer strBuffer1_3 = new StringBuffer("abc123abc");

        StringBuffer strBuffer2_1 = new StringBuffer("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuffer strBuffer2_2 = new StringBuffer(" @!.&%()*");
        StringBuffer strBuffer2_3 = new StringBuffer("abc123abc");

        test1(strBuffer1_1);
        test1(strBuffer1_2);
        test1(strBuffer1_3);

        test2(strBuffer2_1);
        test2(strBuffer2_2);
        test2(strBuffer2_3);
    }

    // Test method delete(int start, int end).
    private static void test1(StringBuffer strBuffer) {
        // Test 0 < start < end, end < strBuffer.length().
        System.out.println(strBuffer.delete(3, 5));
        // Test start = 0.
        System.out.println(strBuffer.delete(0, 1));
        // Test end = 0.
        System.out.println(strBuffer.delete(0, 0));
        // Test start=strBuffer.length().
        System.out.println(strBuffer.delete(strBuffer.length(), strBuffer.length()));
        // Test end=strBuffer.length().
        System.out.println(strBuffer.delete(2, strBuffer.length()));
    }

    // Test method deleteCharAt(int index).
    private static void test2(StringBuffer strBuffer) {
        // Test 0 < index < strBuffer.length().
        System.out.println(strBuffer.deleteCharAt(1));
        // Test index = 0.
        System.out.println(strBuffer.deleteCharAt(0));
        // Test index = strBuffer.length() - 1.
        System.out.println(strBuffer.deleteCharAt(strBuffer.length() - 1));
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full qweyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nweyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nweyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nweyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nwe\n @!%()*\n@!%()*\n@!%()*\n@!%()*\n@!\nabc3abc\nbc3abc\nbc3abc\nbc3abc\nbc\nqertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x9\n !.&%()*\n!.&%()*\n!.&%()\nac123abc\nc123abc\nc123ab\n0\n
