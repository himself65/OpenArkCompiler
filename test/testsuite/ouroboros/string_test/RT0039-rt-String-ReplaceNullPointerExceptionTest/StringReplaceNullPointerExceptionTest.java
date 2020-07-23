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
 * -@TestCaseID: StringReplaceNullPointerExceptionTest.java
 * -@TestCaseName: Test NullPointerException in String Method:String replace(CharSequence target,
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      CharSequence replacement).
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create Parameters: target = null, replacement is a special symbols.
 * -#step3: Test method replace(CharSequence target, CharSequence replacement), NullPointerException is thrown.
 * -#step4: Create Parameters: target is a letter, replacement = null.
 * -#step5: Test method replace(CharSequence target, CharSequence replacement), NullPointerException is thrown.
 * -#step6: Create Parameters: target is a letter, replacement is a letter.
 * -#step7: Test method replace(CharSequence target, CharSequence replacement), the result is replaced correctly.
 * -#step8: Change instance as One or more kinds of letters, numbers, special symbols/""/null to repeat step2~7.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringReplaceNullPointerExceptionTest.java
 * -@ExecuteClass: StringReplaceNullPointerExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringReplaceNullPointerExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringReplaceNullPointerExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringReplaceNullPointerExceptionTest_1() {
        String str1_1 = new String("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ " +
                "ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        String str1_2 = new String(" @!.&%");
        String str1_3 = new String("abc123abc");
        String str1_4 = new String("");
        String str1_5 = new String();

        String str2_1 = "qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ " +
                "ASDFGHJKLQWERTYUIOPZXCVBNM0x96";
        String str2_2 = " @!.&%";
        String str2_3 = "abc123ABC";
        String str2_4 = "";
        String str2_5 = null;

        test(str1_1);
        test(str1_2);
        test(str1_3);
        test(str1_4);
        test(str1_5);

        test(str2_1);
        test(str2_2);
        test(str2_3);
        test(str2_4);
        test(str2_5);
    }

    private static void test(String str) {
        String target = null;
        try {
            System.out.println(str.replace(target, "@"));
        } catch (NullPointerException e1) {
            System.out.println("EXCEPTION 1_1");
        }

        String replacement = null;
        try {
            System.out.println(str.replace("a", replacement));
        } catch (NullPointerException e2) {
            System.out.println("EXCEPTION 1_2");
        }

        try {
            System.out.println(str.replace("a", "@"));
        } catch (NullPointerException e2) {
            System.out.println("EXCEPTION 1_3");
        }
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full EXCEPTION 1_1\nEXCEPTION 1_2\nqwertyuiop{}[]\|@sdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nEXCEPTION 1_1\nEXCEPTION 1_2\n @!.&%\nEXCEPTION 1_1\nEXCEPTION 1_2\n@bc123@bc\nEXCEPTION 1_1\nEXCEPTION 1_2\n\nEXCEPTION 1_1\nEXCEPTION 1_2\n\nEXCEPTION 1_1\nEXCEPTION 1_2\nqwertyuiop{}[]\|@sdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nEXCEPTION 1_1\nEXCEPTION 1_2\n @!.&%\nEXCEPTION 1_1\nEXCEPTION 1_2\n@bc123ABC\nEXCEPTION 1_1\nEXCEPTION 1_2\n\nEXCEPTION 1_1\nEXCEPTION 1_2\nEXCEPTION 1_3\n0\n

