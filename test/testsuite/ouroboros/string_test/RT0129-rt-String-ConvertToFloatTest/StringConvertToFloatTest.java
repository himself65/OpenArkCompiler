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
 * -@TestCaseID: StringConvertToFloatTest.java
 * -@TestCaseName: Change String type to float, test method of Float: public static float parseFloat(String s).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create Parameter s, assign values to String's s.
 * -#step2: Test method parseFloat(String s).
 * -#step3: Check the result get correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringConvertToFloatTest.java
 * -@ExecuteClass: StringConvertToFloatTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringConvertToFloatTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            StringConvertToFloatTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    // Test public static float parseFloat(String s).
    public static void StringConvertToFloatTest_1() {
        float f_1 = Float.parseFloat("12.34");
        System.out.println(f_1);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 12\.34\s*0