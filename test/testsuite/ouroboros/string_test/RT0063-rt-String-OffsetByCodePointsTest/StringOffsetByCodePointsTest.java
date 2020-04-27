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
 * -@TestCaseID: StringOffsetByCodePointsTest.java
 * -@TestCaseName: Test String Method: int offsetByCodePoints(int index, int codePointOffset).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create parameters: index range from 0 to instance.length - 5, codePointOffset = index + 3.
 * -#step3: Test method offsetByCodePoints(int index, int codePointOffset), check the return result is correctly.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~3.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringOffsetByCodePointsTest.java
 * -@ExecuteClass: StringOffsetByCodePointsTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringOffsetByCodePointsTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringOffsetByCodePointsTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringOffsetByCodePointsTest_1() {
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
        for (int i = 0; i < 2; i++) {
            try {
                codePoint = str.offsetByCodePoints(i, i + 3);
                System.out.println("i=" + i + " " + "offsetByCodePoints=" + codePoint);
            } catch (IndexOutOfBoundsException e1) {
                System.out.println("offsetByCodePoints(): " + i + " out of length");
            } finally {
                try {
                    System.out.println(str.charAt(i) + " offsetByCodePoints is " + ":" + codePoint);
                } catch (StringIndexOutOfBoundsException e2) {
                    System.out.println("charAt(): " + i + " out of length");
                }
            }
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan i\=0\s*offsetByCodePoints\=3\s*q\s*offsetByCodePoints\s*is\s*\:3\s*i\=1\s*offsetByCodePoints\=5\s*w\s*offsetByCodePoints\s*is\s*\:5\s*i\=0\s*offsetByCodePoints\=3\s*offsetByCodePoints\s*is\s*\:3\s*i\=1\s*offsetByCodePoints\=5\s*\@\s*offsetByCodePoints\s*is\s*\:5\s*i\=0\s*offsetByCodePoints\=3\s*a\s*offsetByCodePoints\s*is\s*\:3\s*i\=1\s*offsetByCodePoints\=5\s*b\s*offsetByCodePoints\s*is\s*\:5\s*i\=0\s*offsetByCodePoints\=3\s*q\s*offsetByCodePoints\s*is\s*\:3\s*i\=1\s*offsetByCodePoints\=5\s*w\s*offsetByCodePoints\s*is\s*\:5\s*i\=0\s*offsetByCodePoints\=3\s*offsetByCodePoints\s*is\s*\:3\s*i\=1\s*offsetByCodePoints\=5\s*\@\s*offsetByCodePoints\s*is\s*\:5\s*i\=0\s*offsetByCodePoints\=3\s*a\s*offsetByCodePoints\s*is\s*\:3\s*i\=1\s*offsetByCodePoints\=5\s*b\s*offsetByCodePoints\s*is\s*\:5\s*0