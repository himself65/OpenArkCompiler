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
 * -@TestCaseID: AsciiToStringTest.java
 * -@TestCaseName: Test ascii to string By test constructor of String: String(char[] value).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create char[] instance, the element is decimal number.
 * -#step2: Test constructor of String: String(char[] value).
 * -#step3: Check the string create correctly.
 * -#step1: Create char[] instance, the element is hexadecimal number.
 * -#step2: Test constructor of String: String(char[] value).
 * -#step3: Check the string create correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: AsciiToStringTest.java
 * -@ExecuteClass: AsciiToStringTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class AsciiToStringTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            AsciiToStringTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void AsciiToStringTest_1() {
        String str1_1 = "a b^c~AB";
        //10
        char[] data1_2 = {97, 32, 98, 94, 99, 126, 65, 66};
        String str1_2 = new String(data1_2);
        //16
        char[] data1_3 = {0x61, 0x20, 0x62, 0x5E, 0x63, 0x7E, 0x41, 0x42};
        String str1_3 = new String(data1_3);

        test1(str1_1);
        test1(str1_2);
        test1(str1_3);
    }

    private static void test1(String str) {
        System.out.println(str);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full a b^c~AB\na b^c~AB\na b^c~AB\n0\n