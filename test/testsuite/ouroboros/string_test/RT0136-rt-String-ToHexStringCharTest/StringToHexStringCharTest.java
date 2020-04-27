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
 * -@TestCaseID: StringToHexStringCharTest.java
 * -@TestCaseName: Test method toHesString(char ch).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create Parameters: Parameters should be Char type.
 * -#step2: Test method toHesString(char ch).
 * -#step3: Check the return String is correct.
 * -#step4: Replace instance or string which consists of one or more of the following: letters, numbers Contains decimal
 *          and hexadecimal, special symbols, Chinese characters, unicode, then to repeat step2~3.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringToHexStringCharTest.java
 * -@ExecuteClass: StringToHexStringCharTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringToHexStringCharTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringToHexStringCharTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringToHexStringCharTest_1() {
        // 0x00~0x7F.
        String str1_1 = "a b^c~AB";
        //10
        char data1_2[] = {97, 32, 98, 94, 99, 126, 65, 66};
        String str1_2 = new String(data1_2);
        //16
        char data1_3[] = {0x61, 0x20, 0x62, 0x5E, 0x63, 0x7E, 0x41, 0x42};
        String str1_3 = new        String(data1_3);
        String str2_1 = "中国";
        // Test unicode
        char data2_2[] = {'\u4E2D', '\u56FD'};
        String str2_2 = new String(data2_2);
        String str3_1 = "中国abc";
        //unicode
        char data3_2[] = {'\u4E2D', '\u56FD', 97, 98, 99};
        String str3_2 = new String(data3_2);

        test1(str1_1);
        test1(str1_2);
        test1(str1_3);

        test1(str2_1);
        test1(str2_2);
        test1(str3_1);
        test1(str3_2);
    }

    private static void test1(String str) {
        for (int i = 0; i < str.length(); i++) {
            char ch = str.charAt(i);
            System.out.println(Integer.toHexString(ch));
        }
    }
}




// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 61\s*20\s*62\s*5e\s*63\s*7e\s*41\s*42\s*61\s*20\s*62\s*5e\s*63\s*7e\s*41\s*42\s*61\s*20\s*62\s*5e\s*63\s*7e\s*41\s*42\s*4e2d\s*56fd\s*4e2d\s*56fd\s*4e2d\s*56fd\s*61\s*62\s*63\s*4e2d\s*56fd\s*61\s*62\s*63\s*0