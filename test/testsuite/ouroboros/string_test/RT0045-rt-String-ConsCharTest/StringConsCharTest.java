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
 * -@TestCaseID: StringConsCharTest.java
 * -@TestCaseName: Test String constructor: String(char[] value).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create Parameters: value.length > 0, value.element is letter and number.
 * -#step2: Test constructor String(char[] value).
 * -#step3: Check the return String is correctly.
 * -#step4: Create Parameters: value.length = 0.
 * -#step5: Test constructor String(char[] value).
 * -#step6: Check the return String is correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringConsCharTest.java
 * -@ExecuteClass: StringConsCharTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringConsCharTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringConsCharTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringConsCharTest_1() {
        char[] value1_1 = {'a', 'b', 'c', '1', '2', '3'};
        String str1 = new String(value1_1);
        System.out.println(str1);

        char[] value1_2 = {};
        String str2 = new String(value1_2);
        System.out.println(str2);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan abc123\s*0