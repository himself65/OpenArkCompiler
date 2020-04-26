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
 * -@TestCaseID: StringTrimTest.java
 * -@TestCaseName: Test String Method: public String trim().
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Test method trim().
 * -#step3: Check the result get correctly.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols(include\t、\n、\f、\b、\r、\、\\、
 *          \" )/""/NoParam to repeat step2~3.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringTrimTest.java
 * -@ExecuteClass: StringTrimTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringTrimTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            StringTrimTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringTrimTest_1() {
        String str1_1 = new String("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ AS" +
                "DFGHJKLQWERTYUIOPZXCVBNM0x96");
        String str1_2 = new String(" @!.&%");
        String str1_3 = new String(" abc.123.abc ");
        String str1_4 = new String("");
        String str1_5 = new String();

        String str2_1 = "qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ AS" +
                "DFGHJKLQWERTYUIOPZXCVBNM0x96";
        String str2_2 = " @!.&%";
        String str2_3 = " abc.123.ABC ";
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

        char data3_1[] = {'\t', 'a', 'b', 'c', '\t'};
        String str3_1 = new String(data3_1);
        test(str3_1);

        char data3_2[] = {'\n', 'A', 'B', 'C', '\n'};
        String str3_2 = new String(data3_2);
        test(str3_2);

        char data3_3[] = {'\f', 'a', 'b', 'c', '\f'};
        String str3_3 = new String(data3_3);
        test(str3_3);


        char data3_4[] = {'\b', 'A', 'B', 'C', '\b'};
        String str3_4 = new String(data3_4);
        test(str3_4);

        char data3_5[] = {'\r', 'a', 'b', 'c', '\r'};
        String str3_5 = new String(data3_5);
        test(str3_5);

        char data3_6[] = {'\'', 'A', 'B', 'C', '\''};
        String str3_6 = new String(data3_6);
        test(str3_6);

        char data3_7[] = {'\\', 'a', 'b', 'c', '\\'};
        String str3_7 = new String(data3_7);
        test(str3_7);


        char data3_8[] = {'\"', 'A', 'B', 'C', '\"'};
        String str3_8 = new String(data3_8);
        test(str3_8);
    }

    private static void test(String str) {
        System.out.println(str);
        System.out.println(str.trim());
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan qwertyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*qwertyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*\@\!\.\&\%\s*\@\!\.\&\%\s*abc\.123\.abc\s*abc\.123\.abc\s*qwertyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*qwertyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*\@\!\.\&\%\s*\@\!\.\&\%\s*abc\.123\.ABC\s*abc\.123\.ABC\s*abc\s*abc\s*ABC\s*ABC\s*abc\s*abc\s*\ABC\\s*ABC\s*abc\s*abc\s*\'ABC\'\s*\'ABC\'\s*\\abc\\\s*\\abc\\\s*\"ABC\"\s*\"ABC\"\s*0