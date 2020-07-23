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
 * -@TestCaseID: StringConsCharIntIntTest.java
 * -@TestCaseName: Test String constructor: String(char[] value, int offset, int count).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create Parameters: value.length > 0, value.element is letter and number.
 * -#step2: Create Parameters: offset = 0, 0 < count < value.length.
 * -#step3: Test Constructor String(char[] value, int offset, int count), check the return String is correctly.
 * -#step4: Create Parameters: offset = 0, count = 0.
 * -#step5: Test Constructor String(char[] value, int offset, int count), check the return String is correctly.
 * -#step6: Create Parameters: offset = 0, count = value.length.
 * -#step7: Test Constructor String(char[] value, int offset, int count), check the return String is correctly.
 * -#step8: Create Parameters: offset = value.length -1, count = 1.
 * -#step9: Test Constructor String(char[] value, int offset, int count), check the return String is correctly.
 * -#step10: Create Parameters: offset = value.length -1, count = 0.
 * -#step11: Test Constructor String(char[] value, int offset, int count), check the return String is correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringConsCharIntIntTest.java
 * -@ExecuteClass: StringConsCharIntIntTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringConsCharIntIntTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringConsCharIntIntTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringConsCharIntIntTest_1() {
        char[] value1_1 = {'a', 'b', 'c', '1', '2', '3'};
        String str1 = new String(value1_1, 0, 3);
        String str1_1 = new String(value1_1, 0, 0);
        String str1_2 = new String(value1_1, 0, 6);
        String str1_3 = new String(value1_1, 5, 1);
        String str1_4 = new String(value1_1, 5, 0);
        System.out.println(str1);
        System.out.println(str1_1);
        System.out.println(str1_2);
        System.out.println(str1_3);
        System.out.println(str1_4);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full abc\n\nabc123\n3\n\n0\n