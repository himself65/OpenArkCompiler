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
 * -@TestCaseID: StringValueOfCharsTest.java
 * -@TestCaseName: Test String Method: public static String valueOf(char[] data).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create char[] instance as parameter data.
 * -#step2: Test method valueOf(char[] data).
 * -#step3: Check the result get correctly.
 * -#step4: Change data as the char array member type traversal letter, number Contains decimal、 hexadecimal、unix,
 *          empty to repeat step2~3.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringValueOfCharsTest.java
 * -@ExecuteClass: StringValueOfCharsTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringValueOfCharsTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            StringValueOfCharsTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringValueOfCharsTest_1() {
        char[] ch1_1 = {'a', 'b', 'c', '1', '2', '3'};
        char[] ch1_2 = {0x61, 0x62, 0x63, 0x31, 0x32, 0x33};
        char[] ch1_3 = {'\u0061', '\u0062', '\u0063', '\u0031', '\u0032', '\u0033'};
        char[] ch1_4 = {0x0061, 0x0062, 0x0063, 0x0031, 0x0032, 0x0033};
        char[] ch1_5 = {};

        test(ch1_1);
        test(ch1_2);
        test(ch1_3);
        test(ch1_4);
        test(ch1_5);
    }

    private static void test(char[] ch1_1) {
        System.out.println(String.valueOf(ch1_1));
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full abc123\nabc123\nabc123\nabc123\n\n0\n