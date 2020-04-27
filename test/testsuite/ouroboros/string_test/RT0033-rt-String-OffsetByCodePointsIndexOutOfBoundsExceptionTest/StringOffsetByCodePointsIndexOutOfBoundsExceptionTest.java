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
 * -@TestCaseID: StringOffsetByCodePointsIndexOutOfBoundsExceptionTest.java
 * -@TestCaseName: Test IndexOutOfBoundsException in String Method: int offsetByCodePoints(int index,
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      int codePointOffset).
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create Parameters: index range from -1 to instance.length - 4, codePointOffset = index + 3.
 * -#step3: Test Method offsetByCodePoints(int index, int codePointOffset), Check result is correctly.
 *          IndexOutOfBoundsException is thrown when beginIndex < 0 or index + codePointOffset  > instance.length.
 * -#step4: Create Parameters index = beginIndex, Test Method charAt(int index), Check String instance is correctly.
 *          StringIndexOutOfBoundsException is thrown when index < 0 or index >= instance.length.
 * -#step5: Change instance as One or more kinds of letters, numbers, special symbols/""/NoParam to repeat step2~4.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringOffsetByCodePointsIndexOutOfBoundsExceptionTest.java
 * -@ExecuteClass: StringOffsetByCodePointsIndexOutOfBoundsExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringOffsetByCodePointsIndexOutOfBoundsExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringOffsetByCodePointsIndexOutOfBoundsExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringOffsetByCodePointsIndexOutOfBoundsExceptionTest_1() {
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
        for (int i = -1; i < 3; i++) {
            try {
                codePoint = str.offsetByCodePoints(i, i + 3);
                System.out.println("i=" + i + " " + "offsetByCodePoints=" + codePoint);
            } catch (IndexOutOfBoundsException e1) {
                System.out.println("offsetByCodePoints(): " + i + " IndexOutOfBounds");
            } finally {
                try {
                    System.out.println(str.charAt(i) + " offsetByCodePoints is " + ":" + codePoint);
                } catch (StringIndexOutOfBoundsException e2) {
                    System.out.println("charAt(): " + i + " StringIndexOutOfBounds");
                }
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan offsetByCodePoints\(\)\:\s*\-1\s*IndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*i\=0\s*offsetByCodePoints\=3\s*q\s*offsetByCodePoints\s*is\s*\:3\s*i\=1\s*offsetByCodePoints\=5\s*w\s*offsetByCodePoints\s*is\s*\:5\s*i\=2\s*offsetByCodePoints\=7\s*e\s*offsetByCodePoints\s*is\s*\:7\s*offsetByCodePoints\(\)\:\s*\-1\s*IndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*i\=0\s*offsetByCodePoints\=3\s*offsetByCodePoints\s*is\s*\:3\s*i\=1\s*offsetByCodePoints\=5\s*\@\s*offsetByCodePoints\s*is\s*\:5\s*offsetByCodePoints\(\)\:\s*2\s*IndexOutOfBounds\s*\!\s*offsetByCodePoints\s*is\s*\:5\s*offsetByCodePoints\(\)\:\s*\-1\s*IndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*i\=0\s*offsetByCodePoints\=3\s*a\s*offsetByCodePoints\s*is\s*\:3\s*i\=1\s*offsetByCodePoints\=5\s*b\s*offsetByCodePoints\s*is\s*\:5\s*offsetByCodePoints\(\)\:\s*2\s*IndexOutOfBounds\s*c\s*offsetByCodePoints\s*is\s*\:5\s*offsetByCodePoints\(\)\:\s*\-1\s*IndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*offsetByCodePoints\(\)\:\s*0\s*IndexOutOfBounds\s*charAt\(\)\:\s*0\s*StringIndexOutOfBounds\s*offsetByCodePoints\(\)\:\s*1\s*IndexOutOfBounds\s*charAt\(\)\:\s*1\s*StringIndexOutOfBounds\s*offsetByCodePoints\(\)\:\s*2\s*IndexOutOfBounds\s*charAt\(\)\:\s*2\s*StringIndexOutOfBounds\s*offsetByCodePoints\(\)\:\s*\-1\s*IndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*offsetByCodePoints\(\)\:\s*0\s*IndexOutOfBounds\s*charAt\(\)\:\s*0\s*StringIndexOutOfBounds\s*offsetByCodePoints\(\)\:\s*1\s*IndexOutOfBounds\s*charAt\(\)\:\s*1\s*StringIndexOutOfBounds\s*offsetByCodePoints\(\)\:\s*2\s*IndexOutOfBounds\s*charAt\(\)\:\s*2\s*StringIndexOutOfBounds\s*offsetByCodePoints\(\)\:\s*\-1\s*IndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*i\=0\s*offsetByCodePoints\=3\s*q\s*offsetByCodePoints\s*is\s*\:3\s*i\=1\s*offsetByCodePoints\=5\s*w\s*offsetByCodePoints\s*is\s*\:5\s*i\=2\s*offsetByCodePoints\=7\s*e\s*offsetByCodePoints\s*is\s*\:7\s*offsetByCodePoints\(\)\:\s*\-1\s*IndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*i\=0\s*offsetByCodePoints\=3\s*offsetByCodePoints\s*is\s*\:3\s*i\=1\s*offsetByCodePoints\=5\s*\@\s*offsetByCodePoints\s*is\s*\:5\s*offsetByCodePoints\(\)\:\s*2\s*IndexOutOfBounds\s*\!\s*offsetByCodePoints\s*is\s*\:5\s*offsetByCodePoints\(\)\:\s*\-1\s*IndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*i\=0\s*offsetByCodePoints\=3\s*a\s*offsetByCodePoints\s*is\s*\:3\s*i\=1\s*offsetByCodePoints\=5\s*b\s*offsetByCodePoints\s*is\s*\:5\s*offsetByCodePoints\(\)\:\s*2\s*IndexOutOfBounds\s*c\s*offsetByCodePoints\s*is\s*\:5\s*offsetByCodePoints\(\)\:\s*\-1\s*IndexOutOfBounds\s*charAt\(\)\:\s*\-1\s*StringIndexOutOfBounds\s*offsetByCodePoints\(\)\:\s*0\s*IndexOutOfBounds\s*charAt\(\)\:\s*0\s*StringIndexOutOfBounds\s*offsetByCodePoints\(\)\:\s*1\s*IndexOutOfBounds\s*charAt\(\)\:\s*1\s*StringIndexOutOfBounds\s*offsetByCodePoints\(\)\:\s*2\s*IndexOutOfBounds\s*charAt\(\)\:\s*2\s*StringIndexOutOfBounds\s*0