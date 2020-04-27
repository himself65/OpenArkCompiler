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
 * -@TestCaseID: StringValueOfAndCopyValueOfExceptionTest.java
 * -@TestCaseName: Test Exception in String Static Method: String valueOf(char[] data, int offset, int count)/String
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      copyValueOf(char[] data, int offset, int count).
 * -@Brief:
 * -#step1: Create char[] instance initialization by some letters.
 * -#step2: Create Parameters: offset < 0, 0 < count < instance.length.
 * -#step3: Test method valueOf(char[] data, int offset, int count), check IndexOutOfBoundsException is thrown.
 * -#step4: Create Parameters: 0 =< offset < instance.length, count < 0.
 * -#step5: Test method valueOf(char[] data, int offset, int count), check IndexOutOfBoundsException is thrown.
 * -#step6: Create Parameters: 0 =< offset < instance.length, count > instance.length.
 * -#step7: Test method valueOf(char[] data, int offset, int count), check IndexOutOfBoundsException is thrown.
 * -#step8: Create Parameters: offset < 0, 0 < count < instance.length.
 * -#step9: Test method copyValueOf(char[] data, int offset, int count), check IndexOutOfBoundsException is thrown.
 * -#step10: Create Parameters: 0 =< offset < instance.length, count < 0.
 * -#step11: Test method copyValueOf(char[] data, int offset, int count), check IndexOutOfBoundsException is thrown.
 * -#step12: Create Parameters: 0 =< offset < instance.length, count > instance.length.
 * -#step13: Test method copyValueOf(char[] data, int offset, int count), check IndexOutOfBoundsException is thrown.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringValueOfAndCopyValueOfExceptionTest.java
 * -@ExecuteClass: StringValueOfAndCopyValueOfExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringValueOfAndCopyValueOfExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv,  PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            result = StringValueOfAndCopyValueOfExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 93) {
            result = 0;
        }
        return result;
    }

    public static int StringValueOfAndCopyValueOfExceptionTest_1() {
        int result1 = 2; /* STATUS_Success */

        char[] ch1_1 = {'a', 'b', 'c', '1', '2', '3'};
        /* IndexOutOfBoundsException - if offset is negative, or count is negative, or offset+count is larger than
           data.length */
        try {
            String.valueOf(ch1_1, -1, 3);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        try {
            String.valueOf(ch1_1, 1, -3);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        try {
            String.valueOf(ch1_1, 1, 10);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        try {
            String.copyValueOf(ch1_1, -1, 3);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        try {
            String.copyValueOf(ch1_1, 1, -3);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        try {
            String.copyValueOf(ch1_1, 1, 10);
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