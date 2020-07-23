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
 * -@TestCaseID: StringAsVariableAndFieldAndParamTest.java
 * -@TestCaseName: User Scene: class use String.
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create a null String.
 * -#step2: Check the string is create correctly.
 * -#step3: Create a string as the domain value of the class.
 * -#step4: Check the string is create correctly.
 * -#step5: Create a string as input param of function.
 * -#step6: Check the return value of function is correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringAsVariableAndFieldAndParamTest.java
 * -@ExecuteClass: StringAsVariableAndFieldAndParamTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringAsVariableAndFieldAndParamTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringAsVariableAndFieldAndParamTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringAsVariableAndFieldAndParamTest_1() {
        String str = null;
        System.out.println(str);
        System.out.println(StringAsVariableAndFieldAndParamTest_1.str1_1);

        str = "abc";
        System.out.println(StringAsVariableAndFieldAndParamTest_1.test(str));
    }
}

class StringAsVariableAndFieldAndParamTest_1 {
    static String str1_1 = "abc1_1";
    protected static String test(String str) {
        String result = str + str;
        System.out.println(result);
        return result;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full null\nabc1_1\nabcabc\nabcabc\n0\n