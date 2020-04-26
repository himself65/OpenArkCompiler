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
 * -@TestCaseID: StringBufferAppendAndAppendCodePointTest.java
 * -@TestCaseName: Test method append(boolean b)/append(char c)/append(char[] str)/append(CharSequence s)/
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      append(CharSequence s, int start, int end)/append(double d)/append(float f)/append(int i)/
 *                      append(long lng)/append(Object obj)/append(String str)/append(StringBuffer sb)/
 *                      appendCodePoint(int codePoint) of StringBuffer.
 * -@Brief:
 * -#step1: Create StringBuffer instance.
 * -#step2: Create parameter str is a String instance, test method append(String str).
 * -#step3: Create parameter b is true or false, test method append(boolean b).
 * -#step4: Create parameter c is a letter, test method append(char c).
 * -#step5: Create parameter data.length > 0, data.element is a char, test method append(char[] data).
 * -#step6: Create parameter s is a CharSequence, test method append(CharSequence s).
 * -#step7: Create parameters: s is a CharSequence, 0 =< start < end, end <= s.length, start and end coverage boundary
 *          value. test method append(CharSequence s, int start, int end).
 * -#step8: Create parameter d > 0 or d = 0, test method append(double d).
 * -#step9: Create parameter f > 0 or f = 0, test method append(float f).
 * -#step10: Create parameter i > 0 or i = 0, test method append(int i).
 * -#step11: Create parameter lng > 0 or lng = 0, test method append(long lng).
 * -#step12: Create parameter obj is a Object, test method append(Object obj).
 * -#step13: Create parameter sb is a StringBuffer, test method append(StringBuffer sb).
 * -#step14: Create parameter codePoint is unicode num, test method appendCodePoint(int codePoint).
 * -#step15: Check the append result is correctly and the Src StringBuffer is correctly.
 * -#step16: Change instance as One or more kinds of letters, numbers, special symbols/""/NoParam to repeat step2~15.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringBufferAppendAndAppendCodePointTest.java
 * -@ExecuteClass: StringBufferAppendAndAppendCodePointTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringBufferAppendAndAppendCodePointTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringBufferAppendAndAppendCodePointTest_1();
        } catch (Exception e) {
            e.printStackTrace();
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringBufferAppendAndAppendCodePointTest_1() {
        StringBuffer strBuffer1_1 = new StringBuffer("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                                    "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuffer strBuffer1_2 = new StringBuffer(" @!.&%()*");
        StringBuffer strBuffer1_3 = new StringBuffer("abc123abc");
        StringBuffer strBuffer1_4 = new StringBuffer("");
        StringBuffer strBuffer1_5 = new StringBuffer();
        test1(strBuffer1_1);
        test1(strBuffer1_2);
        test1(strBuffer1_3);
        test1(strBuffer1_4);
        test1(strBuffer1_5);
    }

    private static void test1(StringBuffer strBuffer) {
        char c = 'C';
        char[] data = {'A', 'B', 'C'};
        CharSequence chs1_1 = "xyz";
        double d = 8888.8888;
        double dMin = 0;
        float f = 99999999;
        float fMin = 0;
        int i = 77777777;
        int iMin = 0;
        long lng = 66666666;
        long lngMin = 0;
        Object obj = new String("object");
        String str = "string";
        StringBuffer sb = new StringBuffer("stringbuffer");
        int codePoint = 74; // unicode 74 is J

        System.out.println(strBuffer.append("-").append(true).append("-").append(false).append("-").append(c)
                .append("-").append(data).append("-").append(chs1_1).append("-").append(chs1_1, 1, 2).append("-")
                .append(chs1_1, 0, 3).append("-").append(chs1_1, 3, 3).append("-").append(chs1_1, 0, 0).append("-")
                .append(d).append("-").append(dMin).append("-").append(f).append("-").append(fMin).append("-").append(i)
                .append("-").append(iMin).append("-").append(lng).append("-").append(lngMin).append("-").append(obj)
                .append("-").append(str).append("-").append(sb).append("-").appendCodePoint(codePoint));
        System.out.println(strBuffer.subSequence(2, 8));
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan qwertyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\-true\-false\-C\-ABC\-xyz\-y\-xyz\-\-\-8888\.8888\-0\.0\-1\.0E8\-0\.0\-77777777\-0\-66666666\-0\-object\-string\-stringbuffer\-J\s*ertyui\s*\@\!\.\&\%\(\)\*\-true\-false\-C\-ABC\-xyz\-y\-xyz\-\-\-8888\.8888\-0\.0\-1\.0E8\-0\.0\-77777777\-0\-66666666\-0\-object\-string\-stringbuffer\-J\s*\!\.\&\%\(\)\s*abc123abc\-true\-false\-C\-ABC\-xyz\-y\-xyz\-\-\-8888\.8888\-0\.0\-1\.0E8\-0\.0\-77777777\-0\-66666666\-0\-object\-string\-stringbuffer\-J\s*c123ab\s*\-true\-false\-C\-ABC\-xyz\-y\-xyz\-\-\-8888\.8888\-0\.0\-1\.0E8\-0\.0\-77777777\-0\-66666666\-0\-object\-string\-stringbuffer\-J\s*rue\-fa\s*\-true\-false\-C\-ABC\-xyz\-y\-xyz\-\-\-8888\.8888\-0\.0\-1\.0E8\-0\.0\-77777777\-0\-66666666\-0\-object\-string\-stringbuffer\-J\s*rue\-fa\s*0