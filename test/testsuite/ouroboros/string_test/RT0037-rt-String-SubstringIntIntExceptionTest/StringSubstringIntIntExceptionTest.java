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
 * -@TestCaseID: StringSubstringIntIntExceptionTest.java
 * -@TestCaseName: Test StringIndexOutOfBoundsException in String Method: String substring(int beginIndex,
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      int endIndex).
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create Parameters: beginIndex > 0, beginIndex < endIndex < instance.length.
 * -#step3: Test Method substring(int beginIndex,int endIndex), check the result correctly, throws
 *          StringIndexOutOfBoundsException when beginIndex < 0 or beginIndex > instance.length or
 *          endIndex > instance.length.
 * -#step4: Create Parameters: beginIndex < 0, 0 < endIndex < instance.length.
 * -#step5: Test Method substring(int beginIndex,int endIndex), check StringIndexOutOfBoundsException is thrown.
 * -#step6: Create Parameters: beginIndex > endIndex > 0.
 * -#step7: Test Method substring(int beginIndex,int endIndex), check StringIndexOutOfBoundsException is thrown.
 * -#step6: Create Parameters: beginIndex > 0, instance.length < endIndex.
 * -#step7: Test Method substring(int beginIndex,int endIndex), check StringIndexOutOfBoundsException is thrown.
 * -#step8: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~7.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringSubstringIntIntExceptionTest.java
 * -@ExecuteClass: StringSubstringIntIntExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringSubstringIntIntExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringSubstringIntIntExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringSubstringIntIntExceptionTest_1() {
        String str1_1 = new String("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ " +
                "ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        String str1_2 = new String(" @!.&%()*");
        String str1_3 = new String("abc123abc");

        String str2_1 = "qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZ" +
                "XCVBNM0x96";
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
        try {
            System.out.println(str.substring(2, 6));
        } catch (StringIndexOutOfBoundsException e1) {
            System.out.println("EXCEPTION 1_1");
        }

        try {
            System.out.println(str.substring(-1, 6));
        } catch (StringIndexOutOfBoundsException e1) {
            System.out.println("EXCEPTION 1_2");
        }

        try {
            System.out.println(str.substring(2, 1));
        } catch (StringIndexOutOfBoundsException e1) {
            System.out.println("EXCEPTION 1_3");
        }

        try {
            System.out.println(str.substring(2, 9));
        } catch (StringIndexOutOfBoundsException e1) {
            System.out.println("EXCEPTION 1_4");
        }
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full erty\nEXCEPTION 1_2\nEXCEPTION 1_3\nertyuio\n!.&%\nEXCEPTION 1_2\nEXCEPTION 1_3\n!.&%()*\nc123\nEXCEPTION 1_2\nEXCEPTION 1_3\nc123abc\nerty\nEXCEPTION 1_2\nEXCEPTION 1_3\nertyuio\n!.&%\nEXCEPTION 1_2\nEXCEPTION 1_3\nEXCEPTION 1_4\nc123\nEXCEPTION 1_2\nEXCEPTION 1_3\nc123ABC\n0\n

