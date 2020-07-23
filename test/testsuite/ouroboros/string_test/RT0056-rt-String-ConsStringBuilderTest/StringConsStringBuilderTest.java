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
 * -@TestCaseID: StringConsStringBuilderTest.java
 * -@TestCaseName: Test String constructor: String(StringBuilder builder).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String1 by "String" + "String" + "String".
 * -#step2: Create String2 by str2_1 + str2_2 + str2_3.
 * -#step3: Create Parameters: builder by constructor new StringBuilder(String builder).
 * -#step4: Test constructor String(StringBuilder builder).
 * -#step5: check return result is correctly as same as String1 and String2.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringConsStringBuilderTest.java
 * -@ExecuteClass: StringConsStringBuilderTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringConsStringBuilderTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringConsStringBuilderTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringConsStringBuilderTest_1() {
        String str1 = "These " + "are " + "abcdefghijklmnopqrstuvwxyz";
        String str2_1 = "These ";
        String str2_2 = "are ";
        String str2_3 = "abcdefghijklmnopqrstuvwxyz";
        String str2 = str2_1 + str2_2 + str2_3;
        StringBuilder str_b_1 = new StringBuilder("These ").append("are ").append("abcdefghijklmnopqrstuvwxyz");
        String str3 = new String(str_b_1);
        System.out.println(str1);
        System.out.println(str2);
        System.out.println(str3);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full These are abcdefghijklmnopqrstuvwxyz\nThese are abcdefghijklmnopqrstuvwxyz\nThese are abcdefghijklmnopqrstuvwxyz\n0\n