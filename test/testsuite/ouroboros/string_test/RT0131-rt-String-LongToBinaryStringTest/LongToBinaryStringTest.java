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
 * -@TestCaseID: LongToBinaryStringTest.java
 * -@TestCaseName: Test method of Long: static String toBinaryString(long i).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create long instance, value cover 0, normal number, bigger number.
 * -#step2: Test method toBinaryString(long i).
 * -#step3: Check the result get correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: LongToBinaryStringTest.java
 * -@ExecuteClass: LongToBinaryStringTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class LongToBinaryStringTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            LongToBinaryStringTest_1();
            LongToBinaryStringTest_2();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    // Long.toBinaryString(long l)
    public static void LongToBinaryStringTest_1() {
        long l_a = 0L;
        long l_b = 111L;
        long l_c = 2147483647L;

        test(l_a);
        test(l_b);
        test(l_c);
    }

    public static void LongToBinaryStringTest_2() {
        Integer i_a = 0;
        Integer i_b = 111;
        Integer i_c = 2147483647;

        test2(i_a);
        test2(i_b);
        test2(i_c);
    }

    private static void test(Long lo) {
        System.out.println(Long.toBinaryString(lo));
    }

    private static void test2(Integer i) {
        System.out.println(Integer.toBinaryString(i));
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n1101111\n1111111111111111111111111111111\n0\n1101111\n1111111111111111111111111111111\n0\n