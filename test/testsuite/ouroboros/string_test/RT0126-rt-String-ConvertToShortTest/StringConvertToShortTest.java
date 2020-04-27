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
 * -@TestCaseID: StringConvertToShortTest.java
 * -@TestCaseName: Change String type to short, test method of Short: public static short parseShort(String s).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create Parameter s, assign values to String's s.
 * -#step2: Test method parseShort(String s).
 * -#step3: Check the result get correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringConvertToShortTest.java
 * -@ExecuteClass: StringConvertToShortTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringConvertToShortTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            StringConvertToShortTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    // Test public static short parseShort(String s).
    public static void StringConvertToShortTest_1() {
        String str1 = "04320";
        short s_1 = Short.parseShort(str1);
        System.out.println(s_1);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 4320\s*0