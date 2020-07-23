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
 * -@TestCaseID: StringSubstringIntIntTest.java
 * -@TestCaseName: String Method: String substring(int beginIndex, int endIndex).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create parameters: 0 =< beginIndex < endIndex =< instance.length.
 * -#step3: Test method substring(int beginIndex, int endIndex).
 * -#step4: Check the String result is correctly.
 * -#step5: Replace instance or string which consists of one or more of the following: letters, numbers and special
 *          symbols, then to repeat step2~4.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringSubstringIntIntTest.java
 * -@ExecuteClass: StringSubstringIntIntTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringSubstringIntIntTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringSubstringIntIntTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringSubstringIntIntTest_1() {
        String str1_1 = new String("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+A" +
                "SDFGHJKLQWERTYUIOPZXCVBNM0x96");
        String str1_2 = new String(" @!.&%");
        String str1_3 = new String("abc123abc");

        String str2_1 = "qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ASDFGHJKLQWERTYUIOPZXC" +
                "VBNM0x96";
        String str2_2 = " @!.&%";
        String str2_3 = "abc123ABC";

        test(str1_1);
        test(str1_2);
        test(str1_3);

        test(str2_1);
        test(str2_2);
        test(str2_3);
    }

    private static void test(String str) {
        // Test 0 < beginIndex < endIndex < str.length().
        System.out.println(str.substring(2, 5));
        // Test beginIndex = 0.
        System.out.println(str.substring(0, str.length() - 1));
        // Test endIndex = 0.
        System.out.println(str.substring(0, 0));
        // Test beginIndex = str.length() - 1.
        System.out.println(str.substring(str.length() - 1, str.length() - 1));
        // Test endIndex = str.length.
        System.out.println(str.substring(2, str.length() - 1));

    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ert\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ASDFGHJKLQWERTYUIOPZXCVBNM0x9\n\n\nertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ASDFGHJKLQWERTYUIOPZXCVBNM0x9\n!.&\n @!.&\n\n\n!.&\nc12\nabc123ab\n\n\nc123ab\nert\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ASDFGHJKLQWERTYUIOPZXCVBNM0x9\n\n\nertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ASDFGHJKLQWERTYUIOPZXCVBNM0x9\n!.&\n @!.&\n\n\n!.&\nc12\nabc123AB\n\n\nc123AB\n0\n
