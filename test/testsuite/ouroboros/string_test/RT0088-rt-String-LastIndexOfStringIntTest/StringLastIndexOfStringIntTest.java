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
 * -@TestCaseID: StringLastIndexOfStringIntTest.java
 * -@TestCaseName: Test String Method: int lastIndexOf(String str, int fromIndex).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create parameters: str is a letter, 0 =< fromIndex =< instance.length.
 * -#step3: Test method lastIndexOf(String str, int fromIndex).
 * -#step4: Check the int result is correctly.
 * -#step5: Change instance as One or more kinds of letters, numbers, special symbols/""/NoParam to repeat step2~4.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringLastIndexOfStringIntTest.java
 * -@ExecuteClass: StringLastIndexOfStringIntTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringLastIndexOfStringIntTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringLastIndexOfStringIntTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringLastIndexOfStringIntTest_1() {
        String str1_1 = new String("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ASD" +
                "FGHJKLQWERTYUIOPZXCVBNM0x96");
        String str1_2 = new String(" @!.&%");
        String str1_3 = new String("abc123abc");
        String str1_4 = new String("");
        String str1_5 = new String();

        String str2_1 = "qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ASDFGHJKLQWERTYUIOPZX" +
                "CVBNM0x96";
        String str2_2 = " @!.&%";
        String str2_3 = "abc123ABC";
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
        // Test fromIndex = 0.
        System.out.println(str.lastIndexOf("c", 0));
        // Test 0 < fromIndex < str.length().
        System.out.println(str.lastIndexOf("c", 2));
        // Test fromIndex = str.length().
        System.out.println(str.lastIndexOf("c", str.length()));
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan \-1\s*\-1\s*31\s*\-1\s*\-1\s*\-1\s*\-1\s*2\s*8\s*\-1\s*\-1\s*\-1\s*\-1\s*\-1\s*\-1\s*\-1\s*\-1\s*31\s*\-1\s*\-1\s*\-1\s*\-1\s*2\s*2\s*\-1\s*\-1\s*\-1\s*0