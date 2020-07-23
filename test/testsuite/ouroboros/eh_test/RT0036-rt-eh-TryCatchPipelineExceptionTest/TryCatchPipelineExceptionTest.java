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
 * -@TestCaseID: TryCatchPipelineExceptionTest.java
 * -@TestCaseName: Test try-catch(type1 | type 2 | ...).
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: Create a String s include number and special symbol.
 * -#step2: Use try-catch(type1 | type 2 | ...) to test static int parseInt(String s), NumberFormatException is in first
 *          type.
 * -#step3: Check NumberFormatException is thrown.
 * -#case2
 * -#step1: Create a String s include number and special symbol.
 * -#step2: Use try-catch(type1 | type 2 | ...) to test static int parseInt(String s), NumberFormatException is in
 *          second type.
 * -#step3: Check NumberFormatException is thrown.
 * -#case3
 * -#step1: Create a String s include number and special symbol.
 * -#step2: Use try-catch(type1 | type 2 | ...) to test static int parseInt(String s), NumberFormatException is in third
 *          type.
 * -#step3: Check NumberFormatException is thrown.
 * -#case4
 * -#step1: Create a String s include number and special symbol.
 * -#step2: Use try-catch(type1 | type 2 | ...) to test static int parseInt(String s), NumberFormatException is in
 *          fourth type.
 * -#step3: Check NumberFormatException is thrown.
 * -#case5
 * -#step1: Create a String s include number and special symbol.
 * -#step2: Use try-catch(type1 | type 2 | ...) to test static int parseInt(String s), NumberFormatException is in fifth
 *          type.
 * -#step3: Check NumberFormatException is thrown.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: TryCatchPipelineExceptionTest.java
 * -@ExecuteClass: TryCatchPipelineExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.nio.file.InvalidPathException;

public class TryCatchPipelineExceptionTest {
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
            tryCatchPipelineExceptionTest1();
            tryCatchPipelineExceptionTest2();
            tryCatchPipelineExceptionTest3();
            tryCatchPipelineExceptionTest4();
            tryCatchPipelineExceptionTest5();
        } catch (Exception e) {
            processResult -= 10;
            result = 3;
        }

        if (result == 2 && processResult == 94) {
            result = 0;
        }
        return result;
    }

    /**
     * Use try-catch(type1 | type 2 | ...) to test static int parseInt(String s), NumberFormatException is in first type.
     */
    public static void tryCatchPipelineExceptionTest1() {
        String str = "123#456";
        try {
            Integer.parseInt(str);
            processResult -= 10;
        } catch (NumberFormatException
                | ClassCastException
                | InvalidPathException
                | IllegalStateException
                | IndexOutOfBoundsException e) {
            processResult--;
        }
    }

    /**
     * Use try-catch(type1 | type 2 | ...) to test static int parseInt(String s), NumberFormatException is in second type.
     */
    public static void tryCatchPipelineExceptionTest2() {
        String str = "123#456";
        try {
            Integer.parseInt(str);
            processResult -= 10;
        } catch (ClassCastException
                | NumberFormatException
                | InvalidPathException
                | IllegalStateException
                | IndexOutOfBoundsException e) {
            processResult--;
        }
    }

    /**
     * Use try-catch(type1 | type 2 | ...) to test static int parseInt(String s), NumberFormatException is in third type.
     */
    public static void tryCatchPipelineExceptionTest3() {
        String str = "123#456";
        try {
            Integer.parseInt(str);
            processResult -= 10;
        } catch (ClassCastException
                | InvalidPathException
                | NumberFormatException
                | IllegalStateException
                | IndexOutOfBoundsException e) {
            processResult--;
        }
    }

    /**
     * Use try-catch(type1 | type 2 | ...) to test static int parseInt(String s), NumberFormatException is in fourth type.
     */
    public static void tryCatchPipelineExceptionTest4() {
        String str = "123#456";
        try {
            Integer.parseInt(str);
            processResult -= 10;
        } catch (ClassCastException
                | InvalidPathException
                | IllegalStateException
                | NumberFormatException
                | IndexOutOfBoundsException e) {
            processResult--;
        }
    }

    /**
     * Use try-catch(type1 | type 2 | ...) to test static int parseInt(String s), NumberFormatException is in fifth+ type.
     */
    public static void tryCatchPipelineExceptionTest5() {
        String str = "123#456";
        try {
            Integer.parseInt(str);
            processResult -= 10;
        } catch (ClassCastException
                | InvalidPathException
                | IllegalStateException
                | IndexOutOfBoundsException
                | NumberFormatException e) {
            processResult--;
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n