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
 * -@TestCaseID: StringCharAtTest.java
 * -@TestCaseName: String Method: char charAt(int index).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create parameters: instance.length > index >= 0, index cover boundary value.
 * -#step3: Test method charAt(int index), check the return length is correctly.
 * -#step4: judge String stance is null/empty/"" or not.
 * -#step5: Change instance as One or more kinds of letters, numbers, special symbols/""/NoParam to repeat step2~4.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringCharAtTest.java
 * -@ExecuteClass: StringCharAtTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringCharAtTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringCharAtTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringCharAtTest_1() {
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

        test1(str1_1);
        test(str1_1);

        test1(str1_2);
        test(str1_2);

        test1(str1_3);
        test(str1_3);

        test(str1_4);
        test(str1_5);

        test1(str2_1);
        test(str2_1);

        test1(str2_2);
        test(str2_2);

        test1(str2_3);
        test(str2_3);

        test(str2_4);
    }

    private static void test(String str) {
        if (str == null) {
            System.out.println(str + " is:null");
        }
        if (str.isEmpty()) {
            System.out.println(str + " is:isEmpty");
        }
        if (str.equals("")) {
            System.out.println(str + " is:\"\"");
        }
        System.out.println("*****");
    }

    private static void test1(String str) {
        char ch = str.charAt(0);
        System.out.println(ch);
        System.out.println(str.charAt(str.length() - 1));
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan q\s*6\s*\*\*\*\*\*\s*\%\s*\*\*\*\*\*\s*a\s*3\s*\*\*\*\*\*\s*is\:isEmpty\s*is\:\"\"\s*\*\*\*\*\*\s*is\:isEmpty\s*is\:\"\"\s*\*\*\*\*\*\s*q\s*6\s*\*\*\*\*\*\s*\%\s*\*\*\*\*\*\s*a\s*3\s*\*\*\*\*\*\s*is\:isEmpty\s*is\:\"\"\s*\*\*\*\*\*\s*0