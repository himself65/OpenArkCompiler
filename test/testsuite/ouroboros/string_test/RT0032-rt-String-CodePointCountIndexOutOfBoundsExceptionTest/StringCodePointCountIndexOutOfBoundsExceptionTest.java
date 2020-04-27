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
 * -@TestCaseID: StringCodePointCountIndexOutOfBoundsExceptionTest.java
 * -@TestCaseName: Test IndexOutOfBoundsException in int codePointCount(int beginIndex, int endIndex) of String.
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create Parameters: beginIndex range from -1 to instance.length - 2, endIndex = beginIndex + 3.
 * -#step3: Test Method codePointCount(int beginIndex, int endIndex), Check result is correctly.
 *          IndexOutOfBoundsException is thrown when beginIndex < 0 or endIndex > instance.length - 1.
 * -#step4: Create Parameters index = beginIndex, Test Method charAt(int index), Check String instance is correctly.
 *          StringIndexOutOfBoundsException is thrown when index < 0 or index >= instance.length.
 * -#step5: Change instance as One or more kinds of letters, numbers, special symbols/""/NoParam to repeat step2~4.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringCodePointCountIndexOutOfBoundsExceptionTest.java
 * -@ExecuteClass: StringCodePointCountIndexOutOfBoundsExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringCodePointCountIndexOutOfBoundsExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringCodePointCountIndexOutOfBoundsExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringCodePointCountIndexOutOfBoundsExceptionTest_1() {
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
        for (int i = -1; i < 5; i++) {
            try {
                codePoint = str.codePointCount(i, i + 3);
                System.out.println("i=" + i + " " + "codePointCount=" + codePoint);
            } catch (IndexOutOfBoundsException e1) {
                System.out.println("codePointCount(): " + i + " IndexOutOfBounds");
            } finally {
                try {
                    System.out.println(str.charAt(i) + " codePointCount is " + ":" + codePoint);
                } catch (StringIndexOutOfBoundsException e2) {
                    System.out.println("charAt(): " + i + " StringIndexOutOfBounds");
                }
            }
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan codePointCount\(\)\:\s*\-1\s*IndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*i\=0\s*codePointCount\=3\s*q\s*codePointCount\s*is\s*\:3\s*i\=1\s*codePointCount\=3\s*w\s*codePointCount\s*is\s*\:3\s*i\=2\s*codePointCount\=3\s*e\s*codePointCount\s*is\s*\:3\s*i\=3\s*codePointCount\=3\s*r\s*codePointCount\s*is\s*\:3\s*i\=4\s*codePointCount\=3\s*t\s*codePointCount\s*is\s*\:3\s*codePointCount\(\)\:\s*\-1\s*IndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*i\=0\s*codePointCount\=3\s*codePointCount\s*is\s*\:3\s*i\=1\s*codePointCount\=3\s*\@\s*codePointCount\s*is\s*\:3\s*i\=2\s*codePointCount\=3\s*\!\s*codePointCount\s*is\s*\:3\s*i\=3\s*codePointCount\=3\s*\.\s*codePointCount\s*is\s*\:3\s*codePointCount\(\)\:\s*4\s*IndexOutOfBounds\s*\&\s*codePointCount\s*is\s*\:3\s*codePointCount\(\)\:\s*\-1\s*IndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*i\=0\s*codePointCount\=3\s*a\s*codePointCount\s*is\s*\:3\s*i\=1\s*codePointCount\=3\s*b\s*codePointCount\s*is\s*\:3\s*i\=2\s*codePointCount\=3\s*c\s*codePointCount\s*is\s*\:3\s*i\=3\s*codePointCount\=3\s*1\s*codePointCount\s*is\s*\:3\s*codePointCount\(\)\:\s*4\s*IndexOutOfBounds\s*2\s*codePointCount\s*is\s*\:3\s*codePointCount\(\)\:\s*\-1\s*IndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*codePointCount\(\)\:\s*0\s*IndexOutOfBounds\s*charAt\(\)\:\s*0\s*StringIndexOutOfBounds\s*codePointCount\(\)\:\s*1\s*IndexOutOfBounds\s*charAt\(\)\:\s*1\s*StringIndexOutOfBounds\s*codePointCount\(\)\:\s*2\s*IndexOutOfBounds\s*charAt\(\)\:\s*2\s*StringIndexOutOfBounds\s*codePointCount\(\)\:\s*3\s*IndexOutOfBounds\s*charAt\(\)\:\s*3\s*StringIndexOutOfBounds\s*codePointCount\(\)\:\s*4\s*IndexOutOfBounds\s*charAt\(\)\:\s*4\s*StringIndexOutOfBounds\s*codePointCount\(\)\:\s*\-1\s*IndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*codePointCount\(\)\:\s*0\s*IndexOutOfBounds\s*charAt\(\)\:\s*0\s*StringIndexOutOfBounds\s*codePointCount\(\)\:\s*1\s*IndexOutOfBounds\s*charAt\(\)\:\s*1\s*StringIndexOutOfBounds\s*codePointCount\(\)\:\s*2\s*IndexOutOfBounds\s*charAt\(\)\:\s*2\s*StringIndexOutOfBounds\s*codePointCount\(\)\:\s*3\s*IndexOutOfBounds\s*charAt\(\)\:\s*3\s*StringIndexOutOfBounds\s*codePointCount\(\)\:\s*4\s*IndexOutOfBounds\s*charAt\(\)\:\s*4\s*StringIndexOutOfBounds\s*codePointCount\(\)\:\s*\-1\s*IndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*i\=0\s*codePointCount\=3\s*q\s*codePointCount\s*is\s*\:3\s*i\=1\s*codePointCount\=3\s*w\s*codePointCount\s*is\s*\:3\s*i\=2\s*codePointCount\=3\s*e\s*codePointCount\s*is\s*\:3\s*i\=3\s*codePointCount\=3\s*r\s*codePointCount\s*is\s*\:3\s*i\=4\s*codePointCount\=3\s*t\s*codePointCount\s*is\s*\:3\s*codePointCount\(\)\:\s*\-1\s*IndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*i\=0\s*codePointCount\=3\s*codePointCount\s*is\s*\:3\s*i\=1\s*codePointCount\=3\s*\@\s*codePointCount\s*is\s*\:3\s*i\=2\s*codePointCount\=3\s*\!\s*codePointCount\s*is\s*\:3\s*i\=3\s*codePointCount\=3\s*\.\s*codePointCount\s*is\s*\:3\s*codePointCount\(\)\:\s*4\s*IndexOutOfBounds\s*\&\s*codePointCount\s*is\s*\:3\s*codePointCount\(\)\:\s*\-1\s*IndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*i\=0\s*codePointCount\=3\s*a\s*codePointCount\s*is\s*\:3\s*i\=1\s*codePointCount\=3\s*b\s*codePointCount\s*is\s*\:3\s*i\=2\s*codePointCount\=3\s*c\s*codePointCount\s*is\s*\:3\s*i\=3\s*codePointCount\=3\s*1\s*codePointCount\s*is\s*\:3\s*codePointCount\(\)\:\s*4\s*IndexOutOfBounds\s*2\s*codePointCount\s*is\s*\:3\s*codePointCount\(\)\:\s*\-1\s*IndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*codePointCount\(\)\:\s*0\s*IndexOutOfBounds\s*charAt\(\)\:\s*0\s*StringIndexOutOfBounds\s*codePointCount\(\)\:\s*1\s*IndexOutOfBounds\s*charAt\(\)\:\s*1\s*StringIndexOutOfBounds\s*codePointCount\(\)\:\s*2\s*IndexOutOfBounds\s*charAt\(\)\:\s*2\s*StringIndexOutOfBounds\s*codePointCount\(\)\:\s*3\s*IndexOutOfBounds\s*charAt\(\)\:\s*3\s*StringIndexOutOfBounds\s*codePointCount\(\)\:\s*4\s*IndexOutOfBounds\s*charAt\(\)\:\s*4\s*StringIndexOutOfBounds\s*0