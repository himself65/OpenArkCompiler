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
 * -@TestCaseID: StringConsStringTest.java
 * -@TestCaseName: Test String constructor: String(String original).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create Parameters: original as One or more kinds of letters, numbers, special symbols.
 * -#step2: Test constructor String(String original).
 * -#step3: Check the return String is correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringConsStringTest.java
 * -@ExecuteClass: StringConsStringTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringConsStringTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringConsStringTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringConsStringTest_1() {
        String str1_1 = new String("abc123");
        String str1 = new String(str1_1);
        System.out.println(str1);

        String str1_2 = new String(" @!.&%");
        String str2 = new String(str1_2);
        System.out.println(str2);

        String str1_3 = new String("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        String str3 = new String(str1_3);
        System.out.println(str3);

        String str1_4 = new String("");
        String str4 = new String(str1_4);
        System.out.println(str4);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan abc123\s*\@\!\.\&\%\s*qwertyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*0