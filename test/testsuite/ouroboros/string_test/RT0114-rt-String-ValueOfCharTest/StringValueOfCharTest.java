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
 * -@TestCaseID: StringValueOfCharTest.java
 * -@TestCaseName: Test String Method: public static String valueOf(char c).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create char instance as parameter c.
 * -#step2: Test method valueOf(char c).
 * -#step3: Check the result get correctly.
 * -#step4: Change c as char instance traversal letter, number Contains decimal、 hexadecimal、unix to repeat step2~3.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringValueOfCharTest.java
 * -@ExecuteClass: StringValueOfCharTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringValueOfCharTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            StringValueOfCharTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringValueOfCharTest_1() {
        char ch1_1 = 'a';
        char ch1_2 = 0x61;
        char ch1_3 = '\u0061';
        char ch1_4 = 0x0061;

        test(ch1_1);
        test(ch1_2);
        test(ch1_3);
        test(ch1_4);
    }

    private static void test(char ch) {
        System.out.println(String.valueOf(ch));
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full a\na\na\na\n0\n