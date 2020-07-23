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
 * -@TestCaseID: BasicToStringTest.java
 * -@TestCaseName:Test method of Long: static String toString(long i), test method of Integer: static String
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                     toString(int i), test method of Float: static String toString(float f), test method of Double:
 *                     static String toString(double d).
 * -@Brief:
 * -#case1
 * -#step1: Create long instance, value cover 0, normal number, bigger number.
 * -#step2: Test method toString(long i).
 * -#step3: Check the result get correctly.
 * -#case2
 * -#step1: Create Integer instance, value cover 0, normal number, bigger number.
 * -#step2: Test method toString(int i).
 * -#step3: Check the result get correctly.
 * -#case3
 * -#step1: Create float instance, value cover 0, normal number, bigger number.
 * -#step2: Test method toString(float f).
 * -#step3: Check the result get correctly.
 * -#case4
 * -#step1: Create double instance, value cover 0, normal number, bigger number.
 * -#step2: Test method toString(double d).
 * -#step3: Check the result get correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: BasicToStringTest.java
 * -@ExecuteClass: BasicToStringTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class BasicToStringTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            BasicToStringTest_1();
            BasicToStringTest_2();
            BasicToStringTest_3();
            BasicToStringTest_4();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    // Long.toString(long l, int p)
    public static void BasicToStringTest_1() {
        long l_a = 0;
        long l_b = 111;
        long l_c = 2147483647;

        test1(l_a);
        test1(l_b);
        test1(l_c);
    }

    public static void BasicToStringTest_2() {
        Integer i_a = 0;
        Integer i_b = 111;
        Integer i_c = 2147483647;

        test2(i_a);
        test2(i_b);
        test2(i_c);
    }

    public static void BasicToStringTest_3() {
        float f_a = 0f;
        float f_b = 11.1f;
        float f_c = 2147483.647f;

        test3(f_a);
        test3(f_b);
        test3(f_c);
    }

    public static void BasicToStringTest_4() {
        double d_a = 0;
        double d_b = 1.11d;
        double d_c = 21474836.47;

        test4(d_a);
        test4(d_b);
        test4(d_c);
    }

    private static void test1(Long lo) {
        System.out.println(Long.toString(lo));
    }

    private static void test2(Integer i) {
        System.out.println(Integer.toString(i));
    }

    private static void test3(float f) {
        System.out.println(Float.toString(f));
    }

    private static void test4(double f) {
        System.out.println(Double.toString(f));
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n111\n2147483647\n0\n111\n2147483647\n0.0\n11.1\n2147483.8\n0.0\n1.11\n2.147483647E7\n0\n