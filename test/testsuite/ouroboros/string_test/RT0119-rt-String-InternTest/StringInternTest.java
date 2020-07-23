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
 * -@TestCaseID: StringInternTest.java
 * -@TestCaseName: Test String Method: public String intern().
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance as some letters.
 * -#step2: Test method intern().
 * -#step3: Check the result get correctly by compare with Various forms of strings.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringInternTest.java
 * -@ExecuteClass: StringInternTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;


public class StringInternTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String argv[], PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            result = result-StringInternTest_1(); // 2-1=1
        } catch (Exception e) {
            System.out.println(e);
            StringInternTest.processResult = StringInternTest.processResult - 10;
        }

        if (result == 1 && StringInternTest.processResult == 98) {
            result = 0;
        }
        return result;
    }


    public static  int StringInternTest_1() {
        int result1 = 4; /* STATUS_FAILED */
        String str1 = "a";
        String str2 = "b";
        String str3 = "ab";
        String str4 = str1 + str2;
        String str5 = new String("ab");
        String str6 = "a" + "b";
        String str7 = "a" + str2;
        String str8 = new String("ab");
        if (str5.intern() == str3 && str5.intern() != str4 && str5.intern() == str6 && str5.intern() != str7
                && str5.intern() == str8.intern()) {
            StringInternTest.processResult = StringInternTest.processResult - 1;
            return 1;
        }
        StringInternTest.processResult = StringInternTest.processResult - 10;
        return result1;
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n