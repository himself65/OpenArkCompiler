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
 * -@TestCaseID: CharactercodePointCountNullPointerException.java
 * -@TestCaseName: Exception in Character:public static int codePointCount
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1:codePointCount(CharSequence seq, int beginIndex, int endIndex),seq is null
 * -#step2:catch Exception
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: CharactercodePointCountNullPointerException.java
 * -@ExecuteClass: CharactercodePointCountNullPointerException
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class CharactercodePointCountNullPointerException {
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
            result = charactercodePointCountNullPointerException();
        } catch (Exception e) {
            processResult -= 20;
        }

        try {
            result = charactercodePointCountNullPointerException2();
        } catch (Exception e) {
            processResult -= 40;
        }

        if (result == 4 && processResult == 97) {
            result = 0;
        }

        return result;
    }

    /**
     * codePointCount(char[] a, int offset, int count),a is null,catch NullPointerException
     *
     * @return status code
     */
    public static int charactercodePointCountNullPointerException() {
        int result1 = 4; /*STATUS_FAILED*/

        // NullPointerException - if seq is null.
        //
        // public static int codePointCount(char[] a, int offset, int count)
        int offset = 1;
        int count = 4;
        char[] chars = null;
        try {
            int obj = Character.codePointCount(chars, offset, count);
            processResult -= 10;
        } catch (NullPointerException e1) {
            processResult--;
        }

        return result1;
    }

    /**
     * codePointCount(CharSequence seq, int beginIndex, int endIndex), seq is null,catch NullPointerException
     *
     * @return status code
     */
    public static int charactercodePointCountNullPointerException2() {
        int result1 = 4; /*STATUS_FAILED*/

        // NullPointerException - if seq is null.
        //
        // public static int codePointCount(CharSequence seq, int beginIndex, int endIndex)
        int offset = 1;
        int count = 4;
        CharSequence seq = null;
        try {
            int obj = Character.codePointCount(seq, offset, count);
            processResult -= 30;
        } catch (NullPointerException e1) {
            processResult--;
        }

        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n