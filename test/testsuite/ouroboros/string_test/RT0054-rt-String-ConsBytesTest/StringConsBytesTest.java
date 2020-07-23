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
 * -@TestCaseID: StringConsBytesTest.java
 * -@TestCaseName: Test String constructor: String(byte bytes[]).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create Parameters: bytes.length > 0, bytes.element is Hexadecimal number.
 * -#step2: Test constructor String(byte bytes[]).
 * -#step3: Check the return String is correctly.
 * -#step4: Create Parameters: bytes.length > 0, bytes.element is Decimal number.
 * -#step5: Test constructor String(byte bytes[]).
 * -#step6: Check the return String is correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringConsBytesTest.java
 * -@ExecuteClass: StringConsBytesTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringConsBytesTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringConsBytesTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringConsBytesTest_1() {
        byte[] str1_1 = new byte[]{(byte) 0x61, (byte) 0x62, (byte) 0x63, (byte) 0x31, (byte) 0x32, (byte) 0x33};
        String str1 = new String(str1_1);
        byte[] str1_2 = new byte[]{97, 98, 99, 49, 50, 51};
        String str1_3 = new String(str1_2);

        System.out.println(str1);
        System.out.println(str1_3);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full abc123\nabc123\n0\n