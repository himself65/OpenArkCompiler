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
 * -@TestCaseID: StringValueOfCharIntIntTest.java
 * -@TestCaseName: Test String Method: public static String valueOf(char[] data, int offset, int count).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create char[] instance as parameter data.
 * -#step2: Create other parameters: 0 =< offset < data.length, 0 =< count <= data.length, offset/count cover boundary
 *          value.
 * -#step3: Test method valueOf(char[] data, int offset, int count).
 * -#step4: Check the result get correctly.
 * -#step5: Change data as the char array member type traversal letter, number Contains decimal、 hexadecimal、unix,
 *          empty to repeat step2~4.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringValueOfCharIntIntTest.java
 * -@ExecuteClass: StringValueOfCharIntIntTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringValueOfCharIntIntTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            StringValueOfCharIntIntTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringValueOfCharIntIntTest_1() {
        char ch1_1[] = {'a', 'b', 'c', '1', '2', '3'};
        char ch1_2[] = {0x61, 0x62, 0x63, 0x31, 0x32, 0x33};
        char ch1_3[] = {'\u0061', '\u0062', '\u0063', '\u0031', '\u0032', '\u0033'};
        char ch1_4[] = {0x0061, 0x0062, 0x0063, 0x0031, 0x0032, 0x0033};

        test(ch1_1);
        test(ch1_2);
        test(ch1_3);
        test(ch1_4);
    }

    private static void test(char[] ch1_1) {
        System.out.println(String.valueOf(ch1_1, 1, 3));
        // Test offset = 0.
        System.out.println(String.valueOf(ch1_1, 0, 3));
        // Test count = 0.
        System.out.println(String.valueOf(ch1_1, 1, 0));
        // Test count = ch1_1.length.
        System.out.println(String.valueOf(ch1_1, 0, ch1_1.length));
        // Test offset = ch1_1.length - 1.
        System.out.println(String.valueOf(ch1_1, ch1_1.length - 1, 1));
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan bc1\s*abc\s*abc123\s*3\s*bc1\s*abc\s*abc123\s*3\s*bc1\s*abc\s*abc123\s*3\s*bc1\s*abc\s*abc123\s*3\s*0