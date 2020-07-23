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
 * -@TestCaseID: LongToOctalStringTest.java
 * -@TestCaseName: Test method of Long: static String toOctalString(long i).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create long instance, value cover 0, normal number, bigger number.
 * -#step2: Test method toOctalString(long i).
 * -#step3: Check the result get correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: LongToOctalStringTest.java
 * -@ExecuteClass: LongToOctalStringTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class LongToOctalStringTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            LongToOctalStringTest_1();
            LongToOctalStringTest_2();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    // Long.toOctalString(long l)
    public static void LongToOctalStringTest_1() {
        long l_a = 0L;
        long l_b = 111L;
        long l_c = 2147483647L;

        test(l_a);
        test(l_b);
        test(l_c);
    }

    public static void LongToOctalStringTest_2() {
        Integer i_a = 0;
        Integer i_b = 111;
        Integer i_c = 2147483647;

        test2(i_a);
        test2(i_b);
        test2(i_c);
    }

    private static void test(Long lo) {
        System.out.println(Long.toOctalString(lo));
    }

    private static void test2(Integer i) {
        System.out.println(Integer.toOctalString(i));
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n157\n17777777777\n0\n157\n17777777777\n0\n