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
 * -@TestCaseID: StringIsEmptyTest.java
 * -@TestCaseName: Test String Method: boolean isEmpty().
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Test method isEmpty(), check the return value is correctly.
 * -#step3: judge String stance is null/empty/"" or not.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols/""/NoParam to repeat step2~3.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringIsEmptyTest.java
 * -@ExecuteClass: StringIsEmptyTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringIsEmptyTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringIsEmptyTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringIsEmptyTest_1() {
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

        System.out.println(str1_1.isEmpty());
        test(str1_1);

        System.out.println(str1_2.isEmpty());
        test(str1_2);

        System.out.println(str1_3.isEmpty());
        test(str1_3);

        System.out.println(str1_4.isEmpty());
        test(str1_4);

        System.out.println(str1_5.isEmpty());
        test(str1_5);

        System.out.println(str2_1.isEmpty());
        test(str2_1);

        System.out.println(str2_2.isEmpty());
        test(str2_2);

        System.out.println(str2_3.isEmpty());
        test(str2_3);

        System.out.println(str2_4.isEmpty());
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
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan false\s*\*\*\*\*\*\s*false\s*\*\*\*\*\*\s*false\s*\*\*\*\*\*\s*true\s*is\:isEmpty\s*is\:\"\"\s*\*\*\*\*\*\s*true\s*is\:isEmpty\s*is\:\"\"\s*\*\*\*\*\*\s*false\s*\*\*\*\*\*\s*false\s*\*\*\*\*\*\s*false\s*\*\*\*\*\*\s*true\s*is\:isEmpty\s*is\:\"\"\s*\*\*\*\*\*\s*0