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
 * -@TestCaseID: CharacterCodePointCountIndexOutOfBoundsExceptionTest.java
 * -@TestCaseName: Exception in Character: public static int codePointCount(char[] seq, int offset, int count),
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * case1:
 * -#step1: Create char[] seq, create offset >=0, count > seq.length.
 * -#step2: Test method codePointCount(char[] seq, int offset, int count).
 * -#step3: Check that IndexOutOfBoundsException thrown correctly.
 * case2:
 * -#step1: Define CharSequence seq, create offset >=0, count > seq.length.
 * -#step2: Test method codePointCount(CharSequence seq, int beginIndex, int endIndex).
 * -#step3: Check that IndexOutOfBoundsException thrown correctly.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: CharacterCodePointCountIndexOutOfBoundsExceptionTest.java
 * -@ExecuteClass: CharacterCodePointCountIndexOutOfBoundsExceptionTest
 */

import java.io.PrintStream;

public class CharacterCodePointCountIndexOutOfBoundsExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public static int run(String[] argv, PrintStream out) {
        int result = 2; /*STATUS_FAILED*/

        try {
            result = charactercodePointCountIndexOutOfBoundsException1();
        } catch (Exception e) {
            processResult -= 20;
        }

        try {
            result = charactercodePointCountIndexOutOfBoundsException2();
        } catch (Exception e) {
            processResult -= 40;
        }

        if (result == 4 && processResult == 97) {
            result = 0;
        }

        return result;
    }

    /**
     * Test method codePointCount(char[] seq, int offset, int count).
     *
     * @return status code
     */
    public static int charactercodePointCountIndexOutOfBoundsException1() {
        int result1 = 4; /*STATUS_FAILED*/

        char[] chars = new char[]{'a', 'b', 'c', 'd', 'e'};
        int offset = 1;
        int count = 10;
        try {
            int obj = Character.codePointCount(chars, offset, count);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult -= 1;
        }

        return result1;
    }

    /**
     * Test method codePointCount(CharSequence seq, int beginIndex, int endIndex).
     *
     * @return status code
     */
    public static int charactercodePointCountIndexOutOfBoundsException2() {
        int result1 = 4; /*STATUS_FAILED*/

        CharSequence seq = "a b c d e";
        int offset = 1;
        int count = 10;
        try {
            int obj = Character.codePointCount(seq, offset, count);
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