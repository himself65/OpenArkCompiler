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
 * -@TestCaseID: StringCodePointBeforeTest.java
 * -@TestCaseName: Test String Method: int codePointBefore(int index).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create parameters: index range from 0 to instance.length - 1.
 * -#step3: Test method codePointBefore(int index), check the return result is correctly.
 * -#step4: check the String instance is correctly.
 * -#step5: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~4.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringCodePointBeforeTest.java
 * -@ExecuteClass: StringCodePointBeforeTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringCodePointBeforeTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringCodePointBeforeTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringCodePointBeforeTest_1() {
        String str1_1 = new String("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        String str1_2 = new String(" @!.&%");
        String str1_3 = new String("abc123");

        String str2_1 = "qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96";
        String str2_2 = " @!.&%";
        String str2_3 = "abc123";

        test(str1_1);
        test(str1_2);
        test(str1_3);
        test(str2_1);
        test(str2_2);
        test(str2_3);
    }

    private static void test(String str) {
        int codePoint = 0;
        for (int i = 1; i < 6; i++) {
            try {
                codePoint = str.codePointBefore(i);
                System.out.println("i=" + i + " " + "codePointBefore=" + codePoint);
            } catch (StringIndexOutOfBoundsException e1) {
                System.out.println("codePointBefore(): " + i + " out of length");
            } finally {
                try {
                    System.out.println(str.charAt(i) + " codePointBefore is" + ":" + codePoint);
                } catch (StringIndexOutOfBoundsException e2) {
                    System.out.println("charAt(): " + i + " out of length");
                }
            }
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan i\=1\s*codePointBefore\=113\s*w\s*codePointBefore\s*is\:113\s*i\=2\s*codePointBefore\=119\s*e\s*codePointBefore\s*is\:119\s*i\=3\s*codePointBefore\=101\s*r\s*codePointBefore\s*is\:101\s*i\=4\s*codePointBefore\=114\s*t\s*codePointBefore\s*is\:114\s*i\=5\s*codePointBefore\=116\s*y\s*codePointBefore\s*is\:116\s*i\=1\s*codePointBefore\=32\s*\@\s*codePointBefore\s*is\:32\s*i\=2\s*codePointBefore\=64\s*\!\s*codePointBefore\s*is\:64\s*i\=3\s*codePointBefore\=33\s*\.\s*codePointBefore\s*is\:33\s*i\=4\s*codePointBefore\=46\s*\&\s*codePointBefore\s*is\:46\s*i\=5\s*codePointBefore\=38\s*\%\s*codePointBefore\s*is\:38\s*i\=1\s*codePointBefore\=97\s*b\s*codePointBefore\s*is\:97\s*i\=2\s*codePointBefore\=98\s*c\s*codePointBefore\s*is\:98\s*i\=3\s*codePointBefore\=99\s*1\s*codePointBefore\s*is\:99\s*i\=4\s*codePointBefore\=49\s*2\s*codePointBefore\s*is\:49\s*i\=5\s*codePointBefore\=50\s*3\s*codePointBefore\s*is\:50\s*i\=1\s*codePointBefore\=113\s*w\s*codePointBefore\s*is\:113\s*i\=2\s*codePointBefore\=119\s*e\s*codePointBefore\s*is\:119\s*i\=3\s*codePointBefore\=101\s*r\s*codePointBefore\s*is\:101\s*i\=4\s*codePointBefore\=114\s*t\s*codePointBefore\s*is\:114\s*i\=5\s*codePointBefore\=116\s*y\s*codePointBefore\s*is\:116\s*i\=1\s*codePointBefore\=32\s*\@\s*codePointBefore\s*is\:32\s*i\=2\s*codePointBefore\=64\s*\!\s*codePointBefore\s*is\:64\s*i\=3\s*codePointBefore\=33\s*\.\s*codePointBefore\s*is\:33\s*i\=4\s*codePointBefore\=46\s*\&\s*codePointBefore\s*is\:46\s*i\=5\s*codePointBefore\=38\s*\%\s*codePointBefore\s*is\:38\s*i\=1\s*codePointBefore\=97\s*b\s*codePointBefore\s*is\:97\s*i\=2\s*codePointBefore\=98\s*c\s*codePointBefore\s*is\:98\s*i\=3\s*codePointBefore\=99\s*1\s*codePointBefore\s*is\:99\s*i\=4\s*codePointBefore\=49\s*2\s*codePointBefore\s*is\:49\s*i\=5\s*codePointBefore\=50\s*3\s*codePointBefore\s*is\:50\s*0