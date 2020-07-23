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
 * -@TestCaseID: StringValueOfDoubleTest.java
 * -@TestCaseName: Test String Method: public static String valueOf(double d).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create double instance as parameter d.
 * -#step2: Test method valueOf(double d).
 * -#step3: Check the result get correctly.
 * -#step4: Change d as double instance traversal double number to repeat step2~3.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringValueOfDoubleTest.java
 * -@ExecuteClass: StringValueOfDoubleTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringValueOfDoubleTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            StringValueOfDoubleTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringValueOfDoubleTest_1() {
        double d1_1 = 0;
        double d1_2 = 1.23;

        test(d1_1);
        test(d1_2);
    }

    private static void test(double d) {
        String test = String.valueOf(d);
        System.out.println(test);
        System.out.println(test + 13);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0.0\n0.013\n1.23\n1.2313\n0\n