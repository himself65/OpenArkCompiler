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
 * -@TestCaseID: StringSubSequenceIndexOutOfBoundsExceptionTest.java
 * -@TestCaseName: Test IndexOutOfBoundsException in String Method: CharSequence subSequence(int beginIndex,
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      int endIndex).
 * -@Brief:
 * -#step1: Create String instance by new String(String str1) and not new.
 * -#step2: Create Parameters: beginIndex < 0, 0 < endIndex < instance.length.
 * -#step3: Test Method subSequence(int beginIndex, int endIndex), check IndexOutOfBoundsException is thrown
 *          StringIndexOutOfBoundsException.
 * -#step4: Create Parameters: 0 < beginIndex < instance.length, endIndex < 0.
 * -#step5: Test Method subSequence(int beginIndex,int endIndex), check IndexOutOfBoundsException is thrown.
 * -#step6: Create Parameters: beginIndex > endIndex > 0.
 * -#step7: Test Method subSequence(int beginIndex,int endIndex), check IndexOutOfBoundsException is thrown.
 * -#step6: Create Parameters: beginIndex > 0, instance.length < endIndex.
 * -#step7: Test Method subSequence(int beginIndex,int endIndex), check IndexOutOfBoundsException is thrown.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringSubSequenceIndexOutOfBoundsExceptionTest.java
 * -@ExecuteClass: StringSubSequenceIndexOutOfBoundsExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringSubSequenceIndexOutOfBoundsExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv,  PrintStream out) {
        int result = 2; /*STATUS_FAILED*/

        try {
            result = StringSubSequenceIndexOutOfBoundsExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 4 && processResult == 95) {
            result = 0;
        }
        return result;
    }

    public static int StringSubSequenceIndexOutOfBoundsExceptionTest_1() {
        int result1 = 4; /*STATUS_FAILED*/
        //IndexOutOfBoundsException - if the index argument is negative or not less than the length of this string.
        String str1_1 = new String("abc123");

        try {
            str1_1.subSequence(-2, 6);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        try {
            str1_1.subSequence(2, -6);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        try {
            str1_1.subSequence(2, 1);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        try {
            str1_1.subSequence(2, 10);
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