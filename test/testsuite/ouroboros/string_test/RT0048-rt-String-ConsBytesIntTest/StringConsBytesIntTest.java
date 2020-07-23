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
 * -@TestCaseID: StringConsBytesIntTest.java
 * -@TestCaseName: Test String constructor: String(byte[] ascii, int hibyte).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create Parameters: ascii.length > 0, ascii.element is ascii number.
 * -#step2: Create Parameters: hibyte = 0, Test constructor String(byte[] ascii, int hibyte).
 * -#step3: Test constructor String(byte[] ascii).
 * -#step4: Check the two constructor result is same or not.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringConsBytesIntTest.java
 * -@ExecuteClass: StringConsBytesIntTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringConsBytesIntTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringConsBytesIntTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringConsBytesIntTest_1() {
        byte[] ascii1_1 = new byte[]{(byte) 0x61, (byte) 0x62, (byte) 0x63, (byte) 0x31, (byte) 0x32, (byte) 0x33};
        String str1 = new String(ascii1_1, 0);
        String str0 = new String(ascii1_1);
        System.out.println(str0);
        System.out.println(str1);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full abc123\nabc123\n0\n