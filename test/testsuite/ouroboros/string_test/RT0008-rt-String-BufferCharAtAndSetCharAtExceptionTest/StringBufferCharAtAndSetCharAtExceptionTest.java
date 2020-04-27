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
 * -@TestCaseID: StringBufferCharAtAndSetCharAtExceptionTest.java
 * -@TestCaseName: Test Method charAt(int index)/setCharAt(int index, char ch) of StringBuffer and
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      StringIndexOutOfBoundsException occured in charAt/setCharAt.
 * -@Brief:
 * -#case1
 * -#step1: Create StringBuffer instance.
 * -#step2: Create parameters: index range from -1  to exceed instance.length.
 * -#step3: Test method char charAt(int index) and StringIndexOutOfBoundsException is thrown.
 * -#step4: Check the result char is correctly when index is range from 0 to (instance.length -1),
 *          StringIndexOutOfBoundsException is thrown when index < 0 or index > (instance.length -1).
 * -#step5: Change instance as One or more kinds of letters, numbers, special symbols/"" to repeat step2~4.
 * -#case2
 * -#step1: Create StringBuffer instance.
 * -#step2: Create parameters: index range from -1  to exceed instance.length. ch is a Letter char.
 * -#step3: Test Method void setCharAt(int index, char ch) and StringIndexOutOfBoundsException is thrown.
 * -#step4: Check the instance after setCharAt is replaced correctly when index is range from 0 to (instance.length -1),
 *          or StringIndexOutOfBoundsException is thrown when index < 0 or index > (instance.length -1).
 * -#step5: Change instance as One or more kinds of letters, numbers, special symbols/"" to repeat step2~4.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringBufferCharAtAndSetCharAtExceptionTest.java
 * -@ExecuteClass: StringBufferCharAtAndSetCharAtExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringBufferCharAtAndSetCharAtExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringBufferCharAtAndSetCharAtExceptionTest_1();
            StringBufferCharAtAndSetCharAtExceptionTest_2();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringBufferCharAtAndSetCharAtExceptionTest_1() {
        StringBuffer strBuffer1_1 = new StringBuffer("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=" +
                "!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuffer strBuffer1_2 = new StringBuffer(" @!.&%()*");
        StringBuffer strBuffer1_3 = new StringBuffer("abc123");
        StringBuffer strBuffer1_4 = new StringBuffer("");

        test1(strBuffer1_1);
        test1(strBuffer1_2);
        test1(strBuffer1_3);
        test1(strBuffer1_4);
    }

    public static void StringBufferCharAtAndSetCharAtExceptionTest_2() {
        StringBuffer strBuffer1_1 = new StringBuffer("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuffer strBuffer1_2 = new StringBuffer(" @!.&%()*");
        StringBuffer strBuffer1_3 = new StringBuffer("abc123");
        StringBuffer strBuffer1_4 = new StringBuffer("");

        test2(strBuffer1_1);
        test2(strBuffer1_2);
        test2(strBuffer1_3);
        test2(strBuffer1_4);
    }

    // Test method char charAt(int index).
    private static void test1(StringBuffer strBuffer) {
        int charAt = 0;
        for (int i = -1; i < 8; i++) {
            try {
                charAt = strBuffer.charAt(i);
                System.out.println("i=" + i + "  " + "charAt=" + charAt);
            } catch (StringIndexOutOfBoundsException e1) {
                System.out.println("index: " + i + "  String index out of range");
            }
        }
    }

    // Test Method void setCharAt(int index, char ch).
    private static void test2(StringBuffer strBuffer) {
        char ch = 'A';
        for (int i = -1; i < 8; i++) {
            try {
                strBuffer.setCharAt(i, ch);
                System.out.println("i=" + i + "  " + "setcharAt=" + strBuffer);
            } catch (StringIndexOutOfBoundsException e1) {
                System.out.println("index: " + i + "  String index out of range");
            }
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan index\:\s*\-1\s*String\s*index\s*out\s*of\s*range\s*i\=0\s*charAt\=113\s*i\=1\s*charAt\=119\s*i\=2\s*charAt\=101\s*i\=3\s*charAt\=114\s*i\=4\s*charAt\=116\s*i\=5\s*charAt\=121\s*i\=6\s*charAt\=117\s*i\=7\s*charAt\=105\s*index\:\s*\-1\s*String\s*index\s*out\s*of\s*range\s*i\=0\s*charAt\=32\s*i\=1\s*charAt\=64\s*i\=2\s*charAt\=33\s*i\=3\s*charAt\=46\s*i\=4\s*charAt\=38\s*i\=5\s*charAt\=37\s*i\=6\s*charAt\=40\s*i\=7\s*charAt\=41\s*index\:\s*\-1\s*String\s*index\s*out\s*of\s*range\s*i\=0\s*charAt\=97\s*i\=1\s*charAt\=98\s*i\=2\s*charAt\=99\s*i\=3\s*charAt\=49\s*i\=4\s*charAt\=50\s*i\=5\s*charAt\=51\s*index\:\s*6\s*String\s*index\s*out\s*of\s*range\s*index\:\s*7\s*String\s*index\s*out\s*of\s*range\s*index\:\s*\-1\s*String\s*index\s*out\s*of\s*range\s*index\:\s*0\s*String\s*index\s*out\s*of\s*range\s*index\:\s*1\s*String\s*index\s*out\s*of\s*range\s*index\:\s*2\s*String\s*index\s*out\s*of\s*range\s*index\:\s*3\s*String\s*index\s*out\s*of\s*range\s*index\:\s*4\s*String\s*index\s*out\s*of\s*range\s*index\:\s*5\s*String\s*index\s*out\s*of\s*range\s*index\:\s*6\s*String\s*index\s*out\s*of\s*range\s*index\:\s*7\s*String\s*index\s*out\s*of\s*range\s*index\:\s*\-1\s*String\s*index\s*out\s*of\s*range\s*i\=0\s*setcharAt\=Awertyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*i\=1\s*setcharAt\=AAertyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*i\=2\s*setcharAt\=AAArtyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*i\=3\s*setcharAt\=AAAAtyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*i\=4\s*setcharAt\=AAAAAyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*i\=5\s*setcharAt\=AAAAAAuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*i\=6\s*setcharAt\=AAAAAAAiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*i\=7\s*setcharAt\=AAAAAAAAop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*index\:\s*\-1\s*String\s*index\s*out\s*of\s*range\s*i\=0\s*setcharAt\=A\@\!\.\&\%\(\)\*\s*i\=1\s*setcharAt\=AA\!\.\&\%\(\)\*\s*i\=2\s*setcharAt\=AAA\.\&\%\(\)\*\s*i\=3\s*setcharAt\=AAAA\&\%\(\)\*\s*i\=4\s*setcharAt\=AAAAA\%\(\)\*\s*i\=5\s*setcharAt\=AAAAAA\(\)\*\s*i\=6\s*setcharAt\=AAAAAAA\)\*\s*i\=7\s*setcharAt\=AAAAAAAA\*\s*index\:\s*\-1\s*String\s*index\s*out\s*of\s*range\s*i\=0\s*setcharAt\=Abc123\s*i\=1\s*setcharAt\=AAc123\s*i\=2\s*setcharAt\=AAA123\s*i\=3\s*setcharAt\=AAAA23\s*i\=4\s*setcharAt\=AAAAA3\s*i\=5\s*setcharAt\=AAAAAA\s*index\:\s*6\s*String\s*index\s*out\s*of\s*range\s*index\:\s*7\s*String\s*index\s*out\s*of\s*range\s*index\:\s*\-1\s*String\s*index\s*out\s*of\s*range\s*index\:\s*0\s*String\s*index\s*out\s*of\s*range\s*index\:\s*1\s*String\s*index\s*out\s*of\s*range\s*index\:\s*2\s*String\s*index\s*out\s*of\s*range\s*index\:\s*3\s*String\s*index\s*out\s*of\s*range\s*index\:\s*4\s*String\s*index\s*out\s*of\s*range\s*index\:\s*5\s*String\s*index\s*out\s*of\s*range\s*index\:\s*6\s*String\s*index\s*out\s*of\s*range\s*index\:\s*7\s*String\s*index\s*out\s*of\s*range\s*0