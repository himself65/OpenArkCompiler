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
 * -@TestCaseID: StringCodePointBeforeExceptionTest.java
 * -@TestCaseName: Test StringIndexOutOfBoundsException in String Method: int codePointBefore(int index).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create Parameters: index range from -1 to instance.length + 1.
 * -#step3: Check the result int is correctly when index is range from 0 to (instance.length -1),
 *          StringIndexOutOfBoundsException is thrown when index < 0 or index > (instance.length -1).
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols/""/NoParam to repeat step2-3.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringCodePointBeforeExceptionTest.java
 * -@ExecuteClass: StringCodePointBeforeExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringCodePointBeforeExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringCodePointBeforeExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringCodePointBeforeExceptionTest_1() {
        String str1_1 = new String("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        String str1_2 = new String(" @!.&%");
        String str1_3 = new String("abc123");
        String str1_4 = new String("");
        String str1_5 = new String();

        String str2_1 = "qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96";
        String str2_2 = " @!.&%";
        String str2_3 = "abc123";
        String str2_4 = "";

        test(str1_1);
        test(str1_2);
        test(str1_3);
        test(str1_4);
        test(str1_5);
        test(str2_1);
        test(str2_2);
        test(str2_3);
        test(str2_4);
    }

    private static void test(String str) {
        int codePoint = 0;
        for (int i = -1; i < 7; i++) {
            try {
                codePoint = str.codePointBefore(i);
                System.out.println("i=" + i + " " + "codePointBefore=" + codePoint);
            } catch (StringIndexOutOfBoundsException e1) {
                System.out.println("codePointBefore(): " + i + " StringIndexOutOfBounds");
            } finally {
                try {
                    System.out.println(str.charAt(i) + " codePointBefore is" + ":" + codePoint);
                } catch (StringIndexOutOfBoundsException e2) {
                    System.out.println("charAt(): " + i + " StringIndexOutOfBounds");
                }
            }
        }
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan codePointBefore\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*0\s*StringIndexOutOfBounds\s*q\s*codePointBefore\s*is\:0\s*i\=1\s*codePointBefore\=113\s*w\s*codePointBefore\s*is\:113\s*i\=2\s*codePointBefore\=119\s*e\s*codePointBefore\s*is\:119\s*i\=3\s*codePointBefore\=101\s*r\s*codePointBefore\s*is\:101\s*i\=4\s*codePointBefore\=114\s*t\s*codePointBefore\s*is\:114\s*i\=5\s*codePointBefore\=116\s*y\s*codePointBefore\s*is\:116\s*i\=6\s*codePointBefore\=121\s*u\s*codePointBefore\s*is\:121\s*codePointBefore\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*0\s*StringIndexOutOfBounds\s*codePointBefore\s*is\:0\s*i\=1\s*codePointBefore\=32\s*\@\s*codePointBefore\s*is\:32\s*i\=2\s*codePointBefore\=64\s*\!\s*codePointBefore\s*is\:64\s*i\=3\s*codePointBefore\=33\s*\.\s*codePointBefore\s*is\:33\s*i\=4\s*codePointBefore\=46\s*\&\s*codePointBefore\s*is\:46\s*i\=5\s*codePointBefore\=38\s*\%\s*codePointBefore\s*is\:38\s*i\=6\s*codePointBefore\=37\s*charAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*0\s*StringIndexOutOfBounds\s*a\s*codePointBefore\s*is\:0\s*i\=1\s*codePointBefore\=97\s*b\s*codePointBefore\s*is\:97\s*i\=2\s*codePointBefore\=98\s*c\s*codePointBefore\s*is\:98\s*i\=3\s*codePointBefore\=99\s*1\s*codePointBefore\s*is\:99\s*i\=4\s*codePointBefore\=49\s*2\s*codePointBefore\s*is\:49\s*i\=5\s*codePointBefore\=50\s*3\s*codePointBefore\s*is\:50\s*i\=6\s*codePointBefore\=51\s*charAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*0\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*0\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*1\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*2\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*2\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*3\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*3\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*4\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*4\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*5\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*5\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*6\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*0\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*0\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*1\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*2\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*2\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*3\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*3\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*4\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*4\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*5\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*5\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*6\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*0\s*StringIndexOutOfBounds\s*q\s*codePointBefore\s*is\:0\s*i\=1\s*codePointBefore\=113\s*w\s*codePointBefore\s*is\:113\s*i\=2\s*codePointBefore\=119\s*e\s*codePointBefore\s*is\:119\s*i\=3\s*codePointBefore\=101\s*r\s*codePointBefore\s*is\:101\s*i\=4\s*codePointBefore\=114\s*t\s*codePointBefore\s*is\:114\s*i\=5\s*codePointBefore\=116\s*y\s*codePointBefore\s*is\:116\s*i\=6\s*codePointBefore\=121\s*u\s*codePointBefore\s*is\:121\s*codePointBefore\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*0\s*StringIndexOutOfBounds\s*codePointBefore\s*is\:0\s*i\=1\s*codePointBefore\=32\s*\@\s*codePointBefore\s*is\:32\s*i\=2\s*codePointBefore\=64\s*\!\s*codePointBefore\s*is\:64\s*i\=3\s*codePointBefore\=33\s*\.\s*codePointBefore\s*is\:33\s*i\=4\s*codePointBefore\=46\s*\&\s*codePointBefore\s*is\:46\s*i\=5\s*codePointBefore\=38\s*\%\s*codePointBefore\s*is\:38\s*i\=6\s*codePointBefore\=37\s*charAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*0\s*StringIndexOutOfBounds\s*a\s*codePointBefore\s*is\:0\s*i\=1\s*codePointBefore\=97\s*b\s*codePointBefore\s*is\:97\s*i\=2\s*codePointBefore\=98\s*c\s*codePointBefore\s*is\:98\s*i\=3\s*codePointBefore\=99\s*1\s*codePointBefore\s*is\:99\s*i\=4\s*codePointBefore\=49\s*2\s*codePointBefore\s*is\:49\s*i\=5\s*codePointBefore\=50\s*3\s*codePointBefore\s*is\:50\s*i\=6\s*codePointBefore\=51\s*charAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*0\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*0\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*1\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*2\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*2\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*3\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*3\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*4\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*4\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*5\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*5\s*StringIndexOutOfBounds\s*codePointBefore\(\)\:\s*6\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*0