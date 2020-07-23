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
 * -@TestCaseID: TryMultiCatchMatchFirstExceptionTest.java
 * -@TestCaseName: Test try-catch/catch/...(multi-catch) mode.
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: Create a String s include number and special symbol.
 * -#step2: Use try-catch/catch/...(multi-catch) test static int parseInt(String s), NumberFormatException is in first
 *          catch, IllegalArgumentException is in second catch, RuntimeException is in third match, Exception is in
 *          fourth catch.
 * -#step3: Check NumberFormatException is caught.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: TryMultiCatchMatchFirstExceptionTest.java
 * -@ExecuteClass: TryMultiCatchMatchFirstExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class TryMultiCatchMatchFirstExceptionTest {
    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_FAILED */

        result = tryMultiCatchMatchFirstExceptionTest1();
        if (result != 0) {
            return 2;
        }
        return result;
    }

    /**
     * Test try-catch/catch/...(multi-catch) mode.
     *
     * @return status code
     */
    public static int tryMultiCatchMatchFirstExceptionTest1() {
        int result1 = 2; /* STATUS_FAILED */
        String str = "123#456";
        try {
            Integer.parseInt(str);
        } catch (NumberFormatException e) {
            result1 = 0;
        } catch (IllegalArgumentException e) {
            result1 = 2;
        } catch (RuntimeException e) {
            result1 = 2;
        } catch (Exception e) {
            result1 = 2;
        }
        return result1;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n