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
 * -@TestCaseID: TryCatchThrowExceptionTest.java
 * -@TestCaseName: Test throws and throw in try-catch.
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#case1
 * -#step1: Create a String s include number and special symbol.
 * -#step2: Test static int parseInt(String s).
 * -#step3: NumberFormatException is caught by catch, throw NumberFormatException correctly in catch.
 * -#case2
 * -#step1: Create a new Exception extends from Exception.
 * -#step2: Throw the new Exception in method correctly.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: TryCatchThrowExceptionTest.java
 * -@ExecuteClass: TryCatchThrowExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class TryCatchThrowExceptionTest {
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
        int result = 2; /* STATUS_FAILED */

        try {
            result = tryCatchThrowExceptionTest1();
        } catch (NumberFormatException e) {
            processResult--;
        }

        if (processResult == 97) {
            result--;
        }

        try {
            tryCatchThrowExceptionTest3();
        } catch (Exception3 e) {
            processResult--;
        }
        if (processResult == 95) {
            result--;
        }
        return result;
    }

    /**
     * Test static int parseInt(String s), NumberFormatException is caught by catch
     *
     * @return status code
     * @throws NumberFormatException
     */
    public static int tryCatchThrowExceptionTest1() throws NumberFormatException {
        int result1 = 4; /* STATUS_FAILED */
        String str = "123#456";
        try {
            Integer.parseInt(str);
        } catch (NumberFormatException e) {
            processResult--;
            throw e;
        }
        processResult -= 10; // Compiled successfully, but cannot execute.
        return result1;
    }

    /**
     * Create a new Exception extends from Exception.
     *
     * @return status code
     * @throws Exception3
     */
    public static int tryCatchThrowExceptionTest3() throws Exception3 {
        int result3 = 3; /* STATUS_FAILED */
        if (result3 != 4) {
            processResult--;
            throw new Exception3();
        }
        processResult -= 10; // Compiled successfully, but cannot execute.
        return result3;
    }

    static class Exception3 extends Exception {
        Exception3() {
            super("yes");
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n