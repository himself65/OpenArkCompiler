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
 * -@TestCaseID: StringValueOfIntTest.java
 * -@TestCaseName: Test String Method: public static String valueOf(int i).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create int instance as parameter i.
 * -#step2: Test method valueOf(int i).
 * -#step3: Check the result get correctly.
 * -#step4: Change i as int instance traversal number to repeat step2~3.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringValueOfIntTest.java
 * -@ExecuteClass: StringValueOfIntTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringValueOfIntTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            StringValueOfIntTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringValueOfIntTest_1() {
        int in1_1 = 0;
        int in1_2 = 12;

        test(in1_1);
        test(in1_2);
    }

    private static void test(int in) {
        String test = String.valueOf(in);
        System.out.println(test);
        System.out.println(test + 13);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\s*013\s*12\s*1213\s*0