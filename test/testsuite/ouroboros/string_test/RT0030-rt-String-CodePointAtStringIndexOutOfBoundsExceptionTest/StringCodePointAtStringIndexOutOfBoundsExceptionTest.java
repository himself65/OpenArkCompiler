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
 * -@TestCaseID: StringCodePointAtStringIndexOutOfBoundsExceptionTest.java
 * -@TestCaseName: Test StringIndexOutOfBoundsException in String Method: public int codePointAt(int index).
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
 * -@Source: StringCodePointAtStringIndexOutOfBoundsExceptionTest.java
 * -@ExecuteClass: StringCodePointAtStringIndexOutOfBoundsExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringCodePointAtStringIndexOutOfBoundsExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringCodePointAtStringIndexOutOfBoundsExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringCodePointAtStringIndexOutOfBoundsExceptionTest_1() {
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
                codePoint = str.codePointAt(i);
                System.out.println("i=" + i + " " + "codePointAt=" + codePoint);
            } catch (StringIndexOutOfBoundsException e1) {
                System.out.println("codePointAt(): " + i + " StringIndexOutOfBounds");
            } finally {
                try {
                    System.out.println(str.charAt(i) + " Unicode is" + ":" + codePoint);
                } catch (StringIndexOutOfBoundsException e2) {
                    System.out.println("charAt(): " + i + " StringIndexOutOfBounds");
                }
            }
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan codePointAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*i\=0\s*codePointAt\=113\s*q\s*Unicode\s*is\:113\s*i\=1\s*codePointAt\=119\s*w\s*Unicode\s*is\:119\s*i\=2\s*codePointAt\=101\s*e\s*Unicode\s*is\:101\s*i\=3\s*codePointAt\=114\s*r\s*Unicode\s*is\:114\s*i\=4\s*codePointAt\=116\s*t\s*Unicode\s*is\:116\s*i\=5\s*codePointAt\=121\s*y\s*Unicode\s*is\:121\s*i\=6\s*codePointAt\=117\s*u\s*Unicode\s*is\:117\s*codePointAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*i\=0\s*codePointAt\=32\s*Unicode\s*is\:32\s*i\=1\s*codePointAt\=64\s*\@\s*Unicode\s*is\:64\s*i\=2\s*codePointAt\=33\s*\!\s*Unicode\s*is\:33\s*i\=3\s*codePointAt\=46\s*\.\s*Unicode\s*is\:46\s*i\=4\s*codePointAt\=38\s*\&\s*Unicode\s*is\:38\s*i\=5\s*codePointAt\=37\s*\%\s*Unicode\s*is\:37\s*codePointAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*i\=0\s*codePointAt\=97\s*a\s*Unicode\s*is\:97\s*i\=1\s*codePointAt\=98\s*b\s*Unicode\s*is\:98\s*i\=2\s*codePointAt\=99\s*c\s*Unicode\s*is\:99\s*i\=3\s*codePointAt\=49\s*1\s*Unicode\s*is\:49\s*i\=4\s*codePointAt\=50\s*2\s*Unicode\s*is\:50\s*i\=5\s*codePointAt\=51\s*3\s*Unicode\s*is\:51\s*codePointAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*0\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*0\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*1\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*2\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*2\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*3\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*3\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*4\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*4\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*5\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*5\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*0\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*0\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*1\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*2\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*2\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*3\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*3\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*4\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*4\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*5\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*5\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*i\=0\s*codePointAt\=113\s*q\s*Unicode\s*is\:113\s*i\=1\s*codePointAt\=119\s*w\s*Unicode\s*is\:119\s*i\=2\s*codePointAt\=101\s*e\s*Unicode\s*is\:101\s*i\=3\s*codePointAt\=114\s*r\s*Unicode\s*is\:114\s*i\=4\s*codePointAt\=116\s*t\s*Unicode\s*is\:116\s*i\=5\s*codePointAt\=121\s*y\s*Unicode\s*is\:121\s*i\=6\s*codePointAt\=117\s*u\s*Unicode\s*is\:117\s*codePointAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*i\=0\s*codePointAt\=32\s*Unicode\s*is\:32\s*i\=1\s*codePointAt\=64\s*\@\s*Unicode\s*is\:64\s*i\=2\s*codePointAt\=33\s*\!\s*Unicode\s*is\:33\s*i\=3\s*codePointAt\=46\s*\.\s*Unicode\s*is\:46\s*i\=4\s*codePointAt\=38\s*\&\s*Unicode\s*is\:38\s*i\=5\s*codePointAt\=37\s*\%\s*Unicode\s*is\:37\s*codePointAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*i\=0\s*codePointAt\=97\s*a\s*Unicode\s*is\:97\s*i\=1\s*codePointAt\=98\s*b\s*Unicode\s*is\:98\s*i\=2\s*codePointAt\=99\s*c\s*Unicode\s*is\:99\s*i\=3\s*codePointAt\=49\s*1\s*Unicode\s*is\:49\s*i\=4\s*codePointAt\=50\s*2\s*Unicode\s*is\:50\s*i\=5\s*codePointAt\=51\s*3\s*Unicode\s*is\:51\s*codePointAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*0\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*0\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*1\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*1\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*2\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*2\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*3\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*3\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*4\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*4\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*5\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*5\s*StringIndexOutOfBounds\s*codePointAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*charAt\(\)\:\s*6\s*StringIndexOutOfBounds\s*0