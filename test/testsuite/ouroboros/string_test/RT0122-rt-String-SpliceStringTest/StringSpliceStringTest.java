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
 * -@TestCaseID: StringSpliceStringTest.java
 * -@TestCaseName: Create String by one string + another string.
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance str.
 * -#step2: Test Create String by str + str, str + another string.
 * -#step3: Check get the result correctly.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols/""/NoParam/null to repeat step2~3.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringSpliceStringTest.java
 * -@ExecuteClass: StringSpliceStringTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringSpliceStringTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            StringSpliceStringTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringSpliceStringTest_1() {
        String str1_1 = new String("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ AS" +
                "DFGHJKLQWERTYUIOPZXCVBNM0x96");
        String str1_2 = new String(" @!.&%");
        String str1_3 = new String("abc123");
        String str1_4 = new String("");
        String str1_5 = new String();

        String str2_1 = "qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ AS" +
                "DFGHJKLQWERTYUIOPZXCVBNM0x96";
        String str2_2 = " @!.&%";
        String str2_3 = "abc123";
        String str2_4 = "";
        String str2_5 = null;

        test(str1_1);
        test(str1_2);
        test(str1_3);
        test(str1_4);
        test(str1_5);
        test(str2_1);
        test(str2_2);
        test(str2_3);
        test(str2_4);
        test(str2_5);
    }

    private static void test(String str) {
        String test1_1 = "test" + str;
        String test1_2 = str + str;
        System.out.println(test1_1);
        System.out.println(test1_2);
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan testqwertyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*qwertyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96qwertyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*test\s*\@\!\.\&\%\s*\@\!\.\&\%\s*\@\!\.\&\%\s*testabc123\s*abc123abc123\s*test\s*test\s*testqwertyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*qwertyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96qwertyuiop\{\}\[\]\\\|asdfghjkl\;\:\'\"zxcvbnm\,\.\<\>\/\?\~\`1234567890\-\=\!\@\#\$\%\^\&\*\(\)_\+\s*ASDFGHJKLQWERTYUIOPZXCVBNM0x96\s*test\s*\@\!\.\&\%\s*\@\!\.\&\%\s*\@\!\.\&\%\s*testabc123\s*abc123abc123\s*test\s*testnull\s*nullnull\s*0