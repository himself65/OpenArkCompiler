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
// ASSERT: scan-full i=1 codePointBefore=113\nw codePointBefore is:113\ni=2 codePointBefore=119\ne codePointBefore is:119\ni=3 codePointBefore=101\nr codePointBefore is:101\ni=4 codePointBefore=114\nt codePointBefore is:114\ni=5 codePointBefore=116\ny codePointBefore is:116\ni=1 codePointBefore=32\n@ codePointBefore is:32\ni=2 codePointBefore=64\n! codePointBefore is:64\ni=3 codePointBefore=33\n. codePointBefore is:33\ni=4 codePointBefore=46\n& codePointBefore is:46\ni=5 codePointBefore=38\n% codePointBefore is:38\ni=1 codePointBefore=97\nb codePointBefore is:97\ni=2 codePointBefore=98\nc codePointBefore is:98\ni=3 codePointBefore=99\n1 codePointBefore is:99\ni=4 codePointBefore=49\n2 codePointBefore is:49\ni=5 codePointBefore=50\n3 codePointBefore is:50\ni=1 codePointBefore=113\nw codePointBefore is:113\ni=2 codePointBefore=119\ne codePointBefore is:119\ni=3 codePointBefore=101\nr codePointBefore is:101\ni=4 codePointBefore=114\nt codePointBefore is:114\ni=5 codePointBefore=116\ny codePointBefore is:116\ni=1 codePointBefore=32\n@ codePointBefore is:32\ni=2 codePointBefore=64\n! codePointBefore is:64\ni=3 codePointBefore=33\n. codePointBefore is:33\ni=4 codePointBefore=46\n& codePointBefore is:46\ni=5 codePointBefore=38\n% codePointBefore is:38\ni=1 codePointBefore=97\nb codePointBefore is:97\ni=2 codePointBefore=98\nc codePointBefore is:98\ni=3 codePointBefore=99\n1 codePointBefore is:99\ni=4 codePointBefore=49\n2 codePointBefore is:49\ni=5 codePointBefore=50\n3 codePointBefore is:50\n0\n

