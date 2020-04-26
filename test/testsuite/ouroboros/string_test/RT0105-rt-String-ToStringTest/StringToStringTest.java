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
 * -@TestCaseID: StringToStringTest.java
 * -@TestCaseName: Test String Method: public String toString().
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Test method toString().
 * -#step3: Check the result get correctly.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols/""/NoParam to repeat step2~3.
 * -#step5: Create String instance by char[].
 * -#step6: Test method toString(), check the result get correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringToStringTest.java
 * -@ExecuteClass: StringToStringTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringToStringTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            StringToStringTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringToStringTest_1() {
        String str1_1 = new String("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ AS" +
                "DFGHJKLQWERTYUIOPZXCVBNM0x96");
        String str1_2 = new String(" @!.&%");
        String str1_3 = new String("abc123abc");
        String str1_4 = new String("");
        String str1_5 = new String();

        String str2_1 = "qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ AS" +
                "DFGHJKLQWERTYUIOPZXCVBNM0x96";
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

        //new String() input : byte[], char[], StringBuffer, StringBuilder, String, int[](ASCII)
        //byte[] char[] must USE new String
        char data3_1[] = {0x61, 0x62, 0x63};
        String str3_1 = new String(data3_1);
        System.out.println(str3_1);
        System.out.println(str3_1.toString());
    }

    private static void test(String str) {
        System.out.println(str.toString());
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan qwertyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*\@\!\.\&\%\s*abc123abc\s*qwertyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*\@\!\.\&\%\s*abc123ABC\s*abc\s*abc\s*0