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
 * -@TestCaseID: StringFormatStringObjectTest.java
 * -@TestCaseName: Test String Method: public static String format(String format, Object... args).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *
 * -@Brief:
 * -#step1: Create String instance as parameter args.
 * -#step2: Create Parameters: format is a format string as %s.
 * -#step3: Test method format(String format, Object... args).
 * -#step4: Check the result get correctly.
 * -#step5: Change instance as one or more kinds of letters, numbers, special symbols/""/NoParam to repeat step2~4.
 * -#step6: Create Parameters: format is a format string as %c %n, args is 'A'.
 * -#step7: Test method format(String format, Object... args), check the result get correctly.
 * -#step8: Create Parameters: format is a format string as %b %n, args Is an expression whose result is true/false.
 * -#step9: Test method format(String format, Object... args), check the result get correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringFormatStringObjectTest.java
 * -@ExecuteClass: StringFormatStringObjectTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringFormatStringObjectTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            StringFormatStringObjectTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringFormatStringObjectTest_1() {
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

        String str3_1 = null;
        str3_1 = String.format("%c %n", 'A');
        System.out.println(str3_1);
        str3_1 = String.format("%b %n", 3 > 7);
        System.out.println(str3_1);
        str3_1 = String.format("%b %n", 3 < 7);
        System.out.println(str3_1);
    }

    private static void test(String str) {
        String test1 = null;
        test1 = String.format("%s", str);
        System.out.println(test1);
        test1 = String.format("%s %s %s", str, str, str);
        System.out.println(test1);    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full qwertyuiop{}[]\\|asdfghjkl;:\'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nqwertyuiop{}[]\\|asdfghjkl;:\'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96 qwertyuiop{}[]\\|asdfghjkl;:\'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96 qwertyuiop{}[]\\|asdfghjkl;:\'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\n @!.&%\n @!.&%  @!.&%  @!.&%\nabc123abc\nabc123abc abc123abc abc123abc\n\n  \n\n  \nqwertyuiop{}[]\\|asdfghjkl;:\'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nqwertyuiop{}[]\\|asdfghjkl;:\'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96 qwertyuiop{}[]\\|asdfghjkl;:\'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96 qwertyuiop{}[]\\|asdfghjkl;:\'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\n @!.&%\n @!.&%  @!.&%  @!.&%\nabc123ABC\nabc123ABC abc123ABC abc123ABC\n\n  \nA \n\nfalse \n\ntrue \n\n0\n

