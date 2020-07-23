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
 * -@TestCaseID: StringVoluationCharsTest.java
 * -@TestCaseName: Test String create type by new String(char[] value).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create Parameter: the element of value is some letter.
 * -#step2: Test constructor String(char[] value).
 * -#step3: Check the result get correctly.
 * -#step4: Change the element of value is ascii num to repeat step2~3.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringVoluationCharsTest.java
 * -@ExecuteClass: StringVoluationCharsTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringVoluationCharsTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            StringVoluationCharsTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringVoluationCharsTest_1() {
        char[] data2 = {'a', 'b', 'c'};
        String str2 = new String(data2);
        char[] data3 = {0x61, 0x62, 0x63};
        String str3 = new String(data3);
        String str4 = new String("abc");

        System.out.println(str2);
        System.out.println(str3);
        System.out.println(str4);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full abc\nabc\nabc\n0\n