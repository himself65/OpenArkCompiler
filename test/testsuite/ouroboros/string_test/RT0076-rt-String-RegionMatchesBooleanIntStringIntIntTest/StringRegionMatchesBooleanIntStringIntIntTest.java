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
 * -@TestCaseID: StringRegionMatchesBooleanIntStringIntIntTest.java
 * -@TestCaseName: Test String Method: boolean regionMatches(boolean ignoreCase, int toffset, String other, int
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      ooffset, int len).
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create parameters: ignoreCase = true, 0 =< toffset < instance.length, 0 =< ooffset < other.length, 0 =< len
 *          < Min value of other.length - ooffset and instance.length - toffset.
 * -#step3: Test method instance.regionMatches(int toffset, String other, int ooffset, int len), check the boolean
 *          result is correctly.
 * -#step4: Create parameters: ignoreCase = false, 0 =< toffset < instance.length, 0 =< ooffset < other.length, 0 =< len
 *          < Min value of other.length - ooffset and instance.length - toffset.
 * -#step5: Test method instance.regionMatches(int toffset, String other, int ooffset, int len), check the boolean
 *          result is correctly.
 * -#step6: Create parameters: ignoreCase/toffset/ooffset/len cover boundary value.
 * -#step7: Test method instance.regionMatches(int toffset, String other, int ooffset, int len), check the boolean
 *          result is correctly.
 * -#step8: Change instance as One or more kinds of letters, numbers, special symbols/""/NoParam to repeat step2~7.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringRegionMatchesBooleanIntStringIntIntTest.java
 * -@ExecuteClass: StringRegionMatchesBooleanIntStringIntIntTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringRegionMatchesBooleanIntStringIntIntTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringRegionMatchesBooleanIntStringIntIntTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringRegionMatchesBooleanIntStringIntIntTest_1() {
        String str1_1 = new String("abc123_SAME");
        String str1_2 = new String("      _same");
        String str1_3 = new String("abc123_same");
        String str1_4 = new String("");
        String str1_5 = new String();

        String str2_1 = "abc123_SAME";
        String str2_2 = "      _same";
        String str2_3 = "abc123_same";
        String str2_4 = "";

        test(str1_1);
        test(str1_2);
        test(str1_3);
        test(str1_4);
        test(str1_5);

        test(str2_1);
        test(str2_2);
        test(str2_3);
        test(str2_4);
    }

    private static void test(String str) {
        String test = "SAME";
        // Test ooffset = 0, len = Min value of other.length - ooffset and instance.length - toffset.
        System.out.println(str.regionMatches(true, 7, test, 0, 4));
        System.out.println(str.regionMatches(false, 7, test, 0, 4));

        // Test toffset = 0.
        System.out.println(str.regionMatches(true, 0, test, 1, 1));
        System.out.println(str.regionMatches(false, 0, test, 1, 1));

        // Test toffset = str.length() - 1, ooffset = test.length() - 1.
        System.out.println(str.regionMatches(true, str.length() - 1, test, 3, 1));
        System.out.println(str.regionMatches(false, str.length() - 1, test, 3, 1));

        // Test len = 0.
        System.out.println(str.regionMatches(true, 7, test, 0, 0));
        System.out.println(str.regionMatches(false, 7, test, 0, 0));
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan true\s*true\s*true\s*false\s*true\s*true\s*true\s*true\s*true\s*false\s*false\s*false\s*true\s*false\s*true\s*true\s*true\s*false\s*true\s*false\s*true\s*false\s*true\s*true\s*false\s*false\s*false\s*false\s*false\s*false\s*false\s*false\s*false\s*false\s*false\s*false\s*false\s*false\s*false\s*false\s*true\s*true\s*true\s*false\s*true\s*true\s*true\s*true\s*true\s*false\s*false\s*false\s*true\s*false\s*true\s*true\s*true\s*false\s*true\s*false\s*true\s*false\s*true\s*true\s*false\s*false\s*false\s*false\s*false\s*false\s*false\s*false\s*0