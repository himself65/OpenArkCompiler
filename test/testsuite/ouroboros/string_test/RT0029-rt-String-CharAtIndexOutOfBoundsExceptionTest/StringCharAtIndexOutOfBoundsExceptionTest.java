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
 * -@TestCaseID: StringCharAtIndexOutOfBoundsExceptionTest.java
 * -@TestCaseName: Test IndexOutOfBoundsException in String Method: public char charAt(int index).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create Parameters: str1.length > 0, index < 0, Test method charAt(int index), check IndexOutOfBoundsException
 *          is thrown.
 * -#step3: Create Parameters: str1.length = 0, index > 0, Test method charAt(int index), check IndexOutOfBoundsException
 *          is thrown.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringCharAtIndexOutOfBoundsExceptionTest.java
 * -@ExecuteClass: StringCharAtIndexOutOfBoundsExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringCharAtIndexOutOfBoundsExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv,  PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            result = StringCharAtIndexOutOfBoundsExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 97) {
            result = 0;
        }
        return result;
    }

    public static int StringCharAtIndexOutOfBoundsExceptionTest_1() {
        int result1 = 2; /* STATUS_Success */
        // IndexOutOfBoundsException - if the index argument is negative or not less than the length of this string.
        String str1_1 = new String("abc123");
        String str1_4 = new String("");
        try {
            char ch = str1_1.charAt(-2);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        try {
            char ch = str1_4.charAt(2);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0