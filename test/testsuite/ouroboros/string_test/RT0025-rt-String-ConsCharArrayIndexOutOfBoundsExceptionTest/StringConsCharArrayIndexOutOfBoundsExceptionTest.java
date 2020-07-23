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
 * -@TestCaseID: StringConsCharArrayIndexOutOfBoundsExceptionTest.java
 * -@TestCaseName: Test String constructor: String(char[] value, int offset, int count) throw
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      IndexOutOfBoundsException.
 * -@Brief:
 * -#step1: Create Parameters: char[] str1_1,str1_1.length > 0, element is letter or number.
 * -#step2: Create offset < 0, 0 < length < str1_1.length, Test Constructors new String(char[] value, int offset,
 *          int count), check IndexOutOfBoundsException is thrown.
 * -#step3: Create 0 =< offset < str1_1.length, length > str1_1.length, Test Constructors new String(char[] value,
 *          int offset, int count), check IndexOutOfBoundsException is thrown.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringConsCharArrayIndexOutOfBoundsExceptionTest.java
 * -@ExecuteClass: StringConsCharArrayIndexOutOfBoundsExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringConsCharArrayIndexOutOfBoundsExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv,  PrintStream out) {
        int result = 2; /* STATUS_Success */
        try {
            result = StringConsCharArrayIndexOutOfBoundsExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 97) {
            result = 0;
        }
        return result;
    }

    public static int StringConsCharArrayIndexOutOfBoundsExceptionTest_1() {
        int result1 = 2; /* STATUS_Success */
        // IndexOutOfBoundsException - If the offset and length arguments index characters outside the bounds of the
        // bytes array
        char[] str1_1 = {'a', 'b', 'c', '1', '2', '3'};
        try {
            String str1 = new String(str1_1, -1, 3);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }

        try {
            String str2 = new String(str1_1, 0, 10);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        return result1;
    }
}





// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n