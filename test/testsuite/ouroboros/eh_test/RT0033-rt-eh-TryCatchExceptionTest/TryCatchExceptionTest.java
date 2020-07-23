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
 * -@TestCaseID: TryCatchExceptionTest.java
 * -@TestCaseName: Test try-catch mode.
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#case1
 * -#step1: Create a String s include number and special symbol.
 * -#step2: Use try-catch test static int parseInt(String s).
 * -#step3: Check NumberFormatException is thrown.
 * -#case2
 * -#step1: Create a String s include number and special symbol.
 * -#step2: Use try-catch test static int parseInt(String s).
 * -#step3: Check IllegalArgumentException is thrown.
 * -#case3
 * -#step1: Create a String s include number and special symbol.
 * -#step2: Use try-catch test static int parseInt(String s).
 * -#step3: Check RuntimeException is thrown.
 * -#case4
 * -#step1: Create a String s include number and special symbol.
 * -#step2: Use try-catch test static int parseInt(String s).
 * -#step3: Check Exception is thrown.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: TryCatchExceptionTest.java
 * -@ExecuteClass: TryCatchExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class TryCatchExceptionTest {
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
        int result = 2;
        try {
            tryCatchExceptionTest1();
            tryCatchExceptionTest2();
            tryCatchExceptionTest3();
            tryCatchExceptionTest4();
        } catch (Exception e) {
            processResult -= 10;
            result = 3;
        }
        if (result == 2 && processResult == 95) {
            result = 0;
        }
        return result;
    }

    /**
     * Check NumberFormatException
     */
    public static void tryCatchExceptionTest1() {
        String str = "123#456";
        try {
            Integer.parseInt(str);
            processResult -= 10;
        } catch (NumberFormatException e) {
            processResult--;
        }
    }

    /**
     * Check IllegalArgumentException
     */
    public static void tryCatchExceptionTest2() {
        String str = "123#456";
        try {
            Integer.parseInt(str);
            processResult -= 10;
        } catch (IllegalArgumentException e) {
            processResult--;
        }
    }

    /**
     * Check RuntimeException
     */
    public static void tryCatchExceptionTest3() {
        String str = "123#456";
        try {
            Integer.parseInt(str);
            processResult -= 10;
        } catch (RuntimeException e) {
            processResult--;
        }
    }

    /**
     * Check Exception
     */
    public static void tryCatchExceptionTest4() {
        String str = "123#456";
        try {
            Integer.parseInt(str);
            processResult -= 10;
        } catch (Exception e) {
            processResult--;
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n