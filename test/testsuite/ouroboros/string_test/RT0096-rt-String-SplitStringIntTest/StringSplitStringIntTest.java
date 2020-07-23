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
 * -@TestCaseID: StringSplitStringIntTest.java
 * -@TestCaseName: Test String Method: String[] split(String regex, int limit).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create parameters: regex is a String mix with letter/number/special symbols, 0 = < limit =< instance.length.
 * -#step3: Test method split(String regex, int limit).
 * -#step4: Check the String[] result is correctly.
 * -#step5: replace instance or string which consists of one or more of the following: letters, numbers and special
 *          symbols, then to repeat step2~4.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringSplitStringIntTest.java
 * -@ExecuteClass: StringSplitStringIntTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringSplitStringIntTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringSplitStringIntTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringSplitStringIntTest_1() {
        String str1_1 = new String("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ A" +
                "SDFGHJKLQWERTYUIOPZXCVBNM0x96");
        String str1_2 = new String(" @!.&%");
        String str1_3 = new String("abc.123.abc");
        String str1_4 = new String("");
        String str1_5 = new String();

        String str2_1 = "qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIO" +
                "PZXCVBNM0x96";
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
        String[] result;
        // Test 0  < limit < instance.length.
        result = str.split("a", 3);
        for (int i = 0; i < result.length; i++) {
            System.out.println(result[i]);
        }

        // Test limit = instance.length.
        result = str.split("a", str.length());
        for (int i = 0; i < result.length; i++) {
            System.out.println(result[i]);
        }

        // Test limit = 0.
        result = str.split("a", 0);
        for (int i = 0; i < result.length; i++) {
            System.out.println(result[i]);
        }

        // Test regex is a special symbols.
        result = str.split("\\.", 2);
        for (int i = 0; i < result.length; i++) {
            System.out.println(result[i]);
        }
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full qwertyuiop{}[]\|\nsdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nqwertyuiop{}[]\|\nsdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nqwertyuiop{}[]\|\nsdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,\n<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\n @!.&%\n @!.&%\n @!.&%\n @!\n&%\n\nbc.123.\nbc\n\nbc.123.\nbc\n\nbc.123.\nbc\nabc\n123.abc\n\n\n\n\n\n\n\n\nqwertyuiop{}[]\|\nsdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nqwertyuiop{}[]\|\nsdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nqwertyuiop{}[]\|\nsdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,\n<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\n @!.&%\n @!.&%\n @!.&%\n @!\n&%\n\nbc.123.ABC\n\nbc.123.ABC\n\nbc.123.ABC\nabc\n123.ABC\n\n\n\n\n0\n
