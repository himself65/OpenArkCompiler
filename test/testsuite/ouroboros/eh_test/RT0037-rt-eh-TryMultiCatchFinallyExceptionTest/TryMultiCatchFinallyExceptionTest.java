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
 * -@TestCaseID: TryMultiCatchFinallyExceptionTest.java
 * -@TestCaseName: Test try-catch/catch…-finally mode.
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#case1
 * -#step1: Create a String s include number and special symbol.
 * -#step2: Use try-catch/catch…-finally to test static int parseInt(String s), NumberFormatException is in catch.
 * -#step3: Check NumberFormatException is caught, finally is traversed.
 * -#case1
 * -#step1: Create a String s include number and special symbol.
 * -#step2: Use try-catch/catch…-finally to test static int parseInt(String s), NumberFormatException is not in catch.
 * -#step3: Check NumberFormatException is thrown, finally is traversed.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: TryMultiCatchFinallyExceptionTest.java
 * -@ExecuteClass: TryMultiCatchFinallyExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class TryMultiCatchFinallyExceptionTest {
    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public static int run(String[] argv, PrintStream out) {
        int result = 2;

        result = tryMultiCatchFinallyExceptionTest();
        if (result != 0) {
            return 2;
        }

        try {
            result = tryMultiCatchFinallyExceptionTest2();
        } catch (NumberFormatException e) {
            if (result != 0) {
                return 2;
            }
        }
        return result;
    }

    /**
     * Use try-catch/catch…-finally to test static int parseInt(String s), NumberFormatException is in catch.
     *
     * @return status code
     */
    public static int tryMultiCatchFinallyExceptionTest() {
        int result1 = 2; /* STATUS_FAILED */
        String str = "123#456";
        try {
            Integer.parseInt(str);
        } catch (NumberFormatException e) {
            result1--;
        } catch (IllegalArgumentException e) {
            result1 = 2;
        } catch (RuntimeException e) {
            result1 = 2;
        } catch (Exception e) {
            result1 = 2;
        } finally {
            result1--;
        }
        return result1;
    }

    /**
     * Use try-catch/catch…-finally to test static int parseInt(String s), NumberFormatException is not in catch.
     *
     * @return status code
     * @throws NumberFormatException
     */
    public static int tryMultiCatchFinallyExceptionTest2() throws NumberFormatException {
        int result2 = 2; /* STATUS_FAILED */
        String str = "123#456";
        try {
            Integer.parseInt(str);
        } catch (IndexOutOfBoundsException e) {
            result2 = 2;
        } finally {
            result2 = 0;
        }
        return result2;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n