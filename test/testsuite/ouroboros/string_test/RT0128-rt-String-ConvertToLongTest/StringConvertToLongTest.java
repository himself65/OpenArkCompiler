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
 * -@TestCaseID: StringConvertToLongTest.java
 * -@TestCaseName: Change String type to long, test method of Long: static long parseLong(String s),
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      static long parseLong(String s, int radix).
 * -@Brief:
 * -#step1: Create Parameter s, assign values to String's s, radix is 10 represents Decimal.
 * -#step2: Test method parseInt(String s, int radix).
 * -#step3: Check the result get correctly.
 * -#step4: Change s as 0, -0, Positive number, negative number, binary number, letters, change radix correspond cover
 *          10, 16, 2, 27 to repeat step2~3.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringConvertToLongTest.java
 * -@ExecuteClass: StringConvertToLongTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringConvertToLongTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            StringConvertToLongTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    // Test public static long parseLong(String s, int radix).
    public static void StringConvertToLongTest_1() {
        long l_1 = Long.parseLong("1234", 10);
        // Test parses the string with specified radix
        System.out.println(l_1);
        long l_a = Long.parseLong("0", 10);
        System.out.println(l_a);
        long l_b = Long.parseLong("111", 10);
        System.out.println(l_b);
        long l_c = Long.parseLong("-0", 10);
        System.out.println(l_c);
        long l_d = Long.parseLong("-BB", 16);
        System.out.println(l_d);
        long l_e = Long.parseLong("1010110", 2);
        System.out.println(l_e);
        long l_f = Long.parseLong("2147483647", 10);
        System.out.println(l_f);
        long l_g = Long.parseLong("-2147483648", 10);
        System.out.println(l_g);
        long l_h = Long.parseLong("ADMIN", 27);
        System.out.println(l_h);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 1234\s*0\s*111\s*0\s*\-187\s*86\s*2147483647\s*\-2147483648\s*5586836\s*0