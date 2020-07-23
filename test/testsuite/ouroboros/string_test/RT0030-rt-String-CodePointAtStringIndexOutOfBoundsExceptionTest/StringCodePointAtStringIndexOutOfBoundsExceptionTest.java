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
// ASSERT: scan-full codePointAt(): -1 StringIndexOutOfBounds\ncharAt(): -1 StringIndexOutOfBounds\ni=0 codePointAt=113\nq Unicode is:113\ni=1 codePointAt=119\nw Unicode is:119\ni=2 codePointAt=101\ne Unicode is:101\ni=3 codePointAt=114\nr Unicode is:114\ni=4 codePointAt=116\nt Unicode is:116\ni=5 codePointAt=121\ny Unicode is:121\ni=6 codePointAt=117\nu Unicode is:117\ncodePointAt(): -1 StringIndexOutOfBounds\ncharAt(): -1 StringIndexOutOfBounds\ni=0 codePointAt=32\n  Unicode is:32\ni=1 codePointAt=64\n@ Unicode is:64\ni=2 codePointAt=33\n! Unicode is:33\ni=3 codePointAt=46\n. Unicode is:46\ni=4 codePointAt=38\n& Unicode is:38\ni=5 codePointAt=37\n% Unicode is:37\ncodePointAt(): 6 StringIndexOutOfBounds\ncharAt(): 6 StringIndexOutOfBounds\ncodePointAt(): -1 StringIndexOutOfBounds\ncharAt(): -1 StringIndexOutOfBounds\ni=0 codePointAt=97\na Unicode is:97\ni=1 codePointAt=98\nb Unicode is:98\ni=2 codePointAt=99\nc Unicode is:99\ni=3 codePointAt=49\n1 Unicode is:49\ni=4 codePointAt=50\n2 Unicode is:50\ni=5 codePointAt=51\n3 Unicode is:51\ncodePointAt(): 6 StringIndexOutOfBounds\ncharAt(): 6 StringIndexOutOfBounds\ncodePointAt(): -1 StringIndexOutOfBounds\ncharAt(): -1 StringIndexOutOfBounds\ncodePointAt(): 0 StringIndexOutOfBounds\ncharAt(): 0 StringIndexOutOfBounds\ncodePointAt(): 1 StringIndexOutOfBounds\ncharAt(): 1 StringIndexOutOfBounds\ncodePointAt(): 2 StringIndexOutOfBounds\ncharAt(): 2 StringIndexOutOfBounds\ncodePointAt(): 3 StringIndexOutOfBounds\ncharAt(): 3 StringIndexOutOfBounds\ncodePointAt(): 4 StringIndexOutOfBounds\ncharAt(): 4 StringIndexOutOfBounds\ncodePointAt(): 5 StringIndexOutOfBounds\ncharAt(): 5 StringIndexOutOfBounds\ncodePointAt(): 6 StringIndexOutOfBounds\ncharAt(): 6 StringIndexOutOfBounds\ncodePointAt(): -1 StringIndexOutOfBounds\ncharAt(): -1 StringIndexOutOfBounds\ncodePointAt(): 0 StringIndexOutOfBounds\ncharAt(): 0 StringIndexOutOfBounds\ncodePointAt(): 1 StringIndexOutOfBounds\ncharAt(): 1 StringIndexOutOfBounds\ncodePointAt(): 2 StringIndexOutOfBounds\ncharAt(): 2 StringIndexOutOfBounds\ncodePointAt(): 3 StringIndexOutOfBounds\ncharAt(): 3 StringIndexOutOfBounds\ncodePointAt(): 4 StringIndexOutOfBounds\ncharAt(): 4 StringIndexOutOfBounds\ncodePointAt(): 5 StringIndexOutOfBounds\ncharAt(): 5 StringIndexOutOfBounds\ncodePointAt(): 6 StringIndexOutOfBounds\ncharAt(): 6 StringIndexOutOfBounds\ncodePointAt(): -1 StringIndexOutOfBounds\ncharAt(): -1 StringIndexOutOfBounds\ni=0 codePointAt=113\nq Unicode is:113\ni=1 codePointAt=119\nw Unicode is:119\ni=2 codePointAt=101\ne Unicode is:101\ni=3 codePointAt=114\nr Unicode is:114\ni=4 codePointAt=116\nt Unicode is:116\ni=5 codePointAt=121\ny Unicode is:121\ni=6 codePointAt=117\nu Unicode is:117\ncodePointAt(): -1 StringIndexOutOfBounds\ncharAt(): -1 StringIndexOutOfBounds\ni=0 codePointAt=32\n  Unicode is:32\ni=1 codePointAt=64\n@ Unicode is:64\ni=2 codePointAt=33\n! Unicode is:33\ni=3 codePointAt=46\n. Unicode is:46\ni=4 codePointAt=38\n& Unicode is:38\ni=5 codePointAt=37\n% Unicode is:37\ncodePointAt(): 6 StringIndexOutOfBounds\ncharAt(): 6 StringIndexOutOfBounds\ncodePointAt(): -1 StringIndexOutOfBounds\ncharAt(): -1 StringIndexOutOfBounds\ni=0 codePointAt=97\na Unicode is:97\ni=1 codePointAt=98\nb Unicode is:98\ni=2 codePointAt=99\nc Unicode is:99\ni=3 codePointAt=49\n1 Unicode is:49\ni=4 codePointAt=50\n2 Unicode is:50\ni=5 codePointAt=51\n3 Unicode is:51\ncodePointAt(): 6 StringIndexOutOfBounds\ncharAt(): 6 StringIndexOutOfBounds\ncodePointAt(): -1 StringIndexOutOfBounds\ncharAt(): -1 StringIndexOutOfBounds\ncodePointAt(): 0 StringIndexOutOfBounds\ncharAt(): 0 StringIndexOutOfBounds\ncodePointAt(): 1 StringIndexOutOfBounds\ncharAt(): 1 StringIndexOutOfBounds\ncodePointAt(): 2 StringIndexOutOfBounds\ncharAt(): 2 StringIndexOutOfBounds\ncodePointAt(): 3 StringIndexOutOfBounds\ncharAt(): 3 StringIndexOutOfBounds\ncodePointAt(): 4 StringIndexOutOfBounds\ncharAt(): 4 StringIndexOutOfBounds\ncodePointAt(): 5 StringIndexOutOfBounds\ncharAt(): 5 StringIndexOutOfBounds\ncodePointAt(): 6 StringIndexOutOfBounds\ncharAt(): 6 StringIndexOutOfBounds\n0\n

