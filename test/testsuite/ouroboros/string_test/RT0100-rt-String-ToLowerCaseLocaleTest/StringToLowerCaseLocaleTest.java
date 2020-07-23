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
 * -@TestCaseID: StringToLowerCaseLocaleTest.java
 * -@TestCaseName: Test String Method: public String toLowerCase(Locale locale).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create Parameters: Create locale random choose three country.
 * -#step3: Test method toLowerCase(Locale locale).
 * -#step4: Check the result get correctly.
 * -#step5: Change instance as One or more kinds of letters, numbers, special symbols/""/NoParam to repeat step2~4.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringToLowerCaseLocaleTest.java
 * -@ExecuteClass: StringToLowerCaseLocaleTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.util.Locale;


public class StringToLowerCaseLocaleTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringToLowerCaseLocaleTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringToLowerCaseLocaleTest_1() {
        String str1_1 = new String("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ AS" +
                "DFGHJKLQWERTYUIOPZXCVBNM0x96");
        String str1_2 = new String(" @!.&%");
        String str1_3 = new String("abc.123.abc.~!@#$%^&*()_+;'/.?");
        String str1_4 = new String("");
        String str1_5 = new String();

        String str2_1 = "qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ AS" +
                "DFGHJKLQWERTYUIOPZXCVBNM0x96";
        String str2_2 = " @!.&%";
        String str2_3 = "abc.123.ABC";
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
        System.out.println(str.toLowerCase(Locale.CHINESE));
        System.out.println(str.toLowerCase(Locale.ENGLISH));
        System.out.println(str.toLowerCase(Locale.FRENCH));
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full qwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ asdfghjklqwertyuiopzxcvbnm0x96\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ asdfghjklqwertyuiopzxcvbnm0x96\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ asdfghjklqwertyuiopzxcvbnm0x96\n @!.&%\n @!.&%\n @!.&%\nabc.123.abc.~!@#$%^&*()_+;'/.?\nabc.123.abc.~!@#$%^&*()_+;'/.?\nabc.123.abc.~!@#$%^&*()_+;'/.?\n\n\n\n\n\n\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ asdfghjklqwertyuiopzxcvbnm0x96\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ asdfghjklqwertyuiopzxcvbnm0x96\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ asdfghjklqwertyuiopzxcvbnm0x96\n @!.&%\n @!.&%\n @!.&%\nabc.123.abc\nabc.123.abc\nabc.123.abc\n\n\n\n0\n
