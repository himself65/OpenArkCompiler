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
 * -@TestCaseID: StringValueOfFloatTest.java
 * -@TestCaseName: Test String Method: public static String valueOf(float f).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create long instance as parameter f.
 * -#step2: Test method valueOf(float f).
 * -#step3: Check the result get correctly.
 * -#step4: Change f as float instance traversal float number to repeat step2~3.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringValueOfFloatTest.java
 * -@ExecuteClass: StringValueOfFloatTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringValueOfFloatTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            StringValueOfFloatTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringValueOfFloatTest_1() {
        float fl1_1 = 0;
        float fl1_2 = (float) 13.0;

        test(fl1_1);
        test(fl1_2);
    }

    private static void test(float fl) {
        String test = String.valueOf(fl);
        System.out.println(test);
        System.out.println(test + 13);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\.0\s*0\.013\s*13\.0\s*13\.013\s*0