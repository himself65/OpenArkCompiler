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
 * -@TestCaseID: StringBufferIndexOfAndLastIndexOfTest.java
 * -@TestCaseName: Test Method int indexOf(String str)/int indexOf(String str, int fromIndex)/int lastIndexOf(
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      String str)/int lastIndexOf(String str, int fromIndex) of StringBuffer.
 * -@Brief:
 * -#case1
 * -#step1: Create StringBuffer instance.
 * -#step2: Create Parameters: str is String with letter or empty String.
 * -#step3: Test method indexOf(String str), check return result is correctly.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols/""/null to repeat step2~3.
 * -#case2
 * -#step1: Create StringBuffer instance.
 * -#step2: Create Parameters: str is String with letter, instance.length > fromIndex >= 0, fromIndex coverage boundary
 *          value.
 * -#step3: Test method indexOf(String str, int fromIndex), check return result is correctly.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols/""/null to repeat step2~3.
 * -#case3
 * -#step1: Create StringBuffer instance.
 * -#step2: Create Parameters: str is String with letter or empty String.
 * -#step3: Test method lastIndexOf(String str), check return result is correctly.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols/""/null to repeat step2~3.
 * -#case4
 * -#step1: Create StringBuffer instance.
 * -#step2: Create Parameters: str is String with letter, instance.length > fromIndex >= 0, fromIndex coverage boundary
 *          value.
 * -#step3: Test method lastIndexOf(String str, int fromIndex), check return result is correctly.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols/""/null to repeat step2~3.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringBufferIndexOfAndLastIndexOfTest.java
 * -@ExecuteClass: StringBufferIndexOfAndLastIndexOfTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringBufferIndexOfAndLastIndexOfTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringBufferIndexOfAndLastIndexOfTest_1();
            StringBufferIndexOfAndLastIndexOfTest_2();
            StringBufferIndexOfAndLastIndexOfTest_3();
            StringBufferIndexOfAndLastIndexOfTest_4();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringBufferIndexOfAndLastIndexOfTest_1() {
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

    public static void StringBufferIndexOfAndLastIndexOfTest_2() {
        StringBuffer strBuffer1_1 = new StringBuffer("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuffer strBuffer1_2 = new StringBuffer(" @!.&%()*");
        StringBuffer strBuffer1_3 = new StringBuffer("abc123abc");
        StringBuffer strBuffer1_4 = new StringBuffer("");
        StringBuffer strBuffer1_5 = new StringBuffer();
        test2(strBuffer1_1);
        test2(strBuffer1_2);
        test2(strBuffer1_3);
        test2(strBuffer1_4);
        test2(strBuffer1_5);
    }

    public static void StringBufferIndexOfAndLastIndexOfTest_3() {
        StringBuffer strBuffer1_1 = new StringBuffer("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuffer strBuffer1_2 = new StringBuffer(" @!.&%()*");
        StringBuffer strBuffer1_3 = new StringBuffer("abc123abc");
        StringBuffer strBuffer1_4 = new StringBuffer("");
        StringBuffer strBuffer1_5 = new StringBuffer();

        test3(strBuffer1_1);
        test3(strBuffer1_2);
        test3(strBuffer1_3);
        test3(strBuffer1_4);
        test3(strBuffer1_5);
    }


    public static void StringBufferIndexOfAndLastIndexOfTest_4() {
        StringBuffer strBuffer1_1 = new StringBuffer("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuffer strBuffer1_2 = new StringBuffer(" @!.&%()*");
        StringBuffer strBuffer1_3 = new StringBuffer("abc123abc");
        StringBuffer strBuffer1_4 = new StringBuffer("");
        StringBuffer strBuffer1_5 = new StringBuffer();

        test4(strBuffer1_1);
        test4(strBuffer1_2);
        test4(strBuffer1_3);
        test4(strBuffer1_4);
        test4(strBuffer1_5);
    }

    // Test method indexOf(String str).
    private static void test1(StringBuffer strBuffer) {
        System.out.println(strBuffer.indexOf("b"));
        // Test empty String.
        System.out.println(strBuffer.indexOf(""));
    }

    // Test method indexOf(String str, int fromIndex).
    private static void test2(StringBuffer strBuffer) {
        System.out.println(strBuffer.indexOf("b", 2));
        // Test fromIndex = 0.
        System.out.println(strBuffer.indexOf("b", 0));
        // Test fromIndex = strBuffer.length() - 1.
        System.out.println(strBuffer.indexOf("6", strBuffer.length() - 1));
    }

    // Test method lastIndexOf(String str).
    private static void test3(StringBuffer strBuffer) {
        System.out.println(strBuffer.lastIndexOf("b"));
        // Test empty String.
        System.out.println(strBuffer.lastIndexOf(""));
    }

    // Test method lastIndexOf(String str, int fromIndex).
    private static void test4(StringBuffer strBuffer) {
        System.out.println(strBuffer.lastIndexOf("b", 1));
        // Test fromIndex = 0.
        System.out.println(strBuffer.lastIndexOf("b", 0));
        // Test fromIndex = strBuffer.length() - 1.
        System.out.println(strBuffer.lastIndexOf("6", strBuffer.length() - 1));
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 33\n0\n-1\n0\n1\n0\n-1\n0\n-1\n0\n33\n33\n98\n-1\n-1\n-1\n7\n1\n-1\n-1\n-1\n-1\n-1\n-1\n-1\n33\n99\n-1\n9\n7\n9\n-1\n0\n-1\n0\n-1\n-1\n98\n-1\n-1\n-1\n1\n-1\n-1\n-1\n-1\n-1\n-1\n-1\n-1\n0\n