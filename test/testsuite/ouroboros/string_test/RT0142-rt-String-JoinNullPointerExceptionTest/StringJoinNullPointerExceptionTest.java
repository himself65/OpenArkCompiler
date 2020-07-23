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
 * -@TestCaseID: StringJoinNullPointerExceptionTest.java
 * -@TestCaseName: Test NullPointerException throw in String Method: static String join(CharSequence delimiter,
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      CharSequence... elements).
 * -@Brief:
 * -#step1: Create Parameters: delimiter is a null CharSequence, elements is a String array, element is null.
 * -#step2: Test method join(CharSequence delimiter, CharSequence... elements).
 * -#step3: NullPointerException is thrown.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: StringJoinNullPointerExceptionTest.java
 * -@ExecuteClass: StringJoinNullPointerExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringJoinNullPointerExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2;  /* STATUS_Success */
        try {
            StringJoinNullPointerExceptionTest_1(null);
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 97) {
            result = 0;
        }
        return result;
    }

    private static void StringJoinNullPointerExceptionTest_1(String str) {
        try {
            String[] test = new String[]{str, str, str};
            System.out.println(String.join(null, test));
        } catch (NullPointerException e) {
            processResult -= 2;
        }
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n