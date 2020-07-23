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
 * -@TestCaseID: StringConvertToIntTest.java
 * -@TestCaseName: Change String type to int, test method of Integer: static int parseInt(String s), static int
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      parseInt(String s, int radix).
 * -@Brief:
 * -#case1
 * -#step1: Create Parameter s, assign values to String's s.
 * -#step2: Test method parseInt(String s).
 * -#step3: Check the result get correctly.
 * -#case2
 * -#step1: Create Parameter s, assign values to String's s, radix is 8 represents octal.
 * -#step2: Test method parseInt(String s, int radix).
 * -#step3: Check the result get correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringConvertToIntTest.java
 * -@ExecuteClass: StringConvertToIntTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringConvertToIntTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            StringConvertToIntTest_1();
            StringConvertToIntTest_2();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    // Test static int parseInt(String s).
    public static void StringConvertToIntTest_1() {
        int i_1 = Integer.parseInt("1234");
        System.out.println("convert(10) to int: " + i_1);
    }

    // Test static int parseInt(String s, int radix).
    public static void StringConvertToIntTest_2() {
        int i_2 = Integer.parseInt("1234", 8);
        System.out.println("convert(8) to int: " + i_2);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full convert(10) to int: 1234\nconvert(8) to int: 668\n0\n