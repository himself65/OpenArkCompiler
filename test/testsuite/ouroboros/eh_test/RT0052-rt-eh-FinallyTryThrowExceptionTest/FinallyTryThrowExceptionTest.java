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
 * -@TestCaseID: FinallyTryThrowExceptionTest.java
 * -@TestCaseName: Test try-catch-finally{try{exception},catch(x)}--> caller.
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: Create a String s include number and special symbol.
 * -#step2: Test static int parseInt(String s) in try-catch.
 * -#step3: Check NumberFormatException is caught by catch.
 * -#step4: Create parameter beginIndex < 0, test method String substring(int beginIndex) in finally.
 * -#step5: Check StringIndexOutOfBoundsException is thrown correctly.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: FinallyTryThrowExceptionTest.java
 * -@ExecuteClass: FinallyTryThrowExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class FinallyTryThrowExceptionTest {
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
            result = finallyTryThrowExceptionTest1();
        } catch (StringIndexOutOfBoundsException e) {
            processResult -= 3;
        }

        if (result == 2 && processResult == 95) {
            result = 0;
        }
        return result;
    }

    /**
     * Test try-catch-finally{try{exception},catch(x)}--> caller.
     *
     * @return status code
     */
    public static int finallyTryThrowExceptionTest1() {
        int result1 = 4; /* STATUS_FAILED */
        String str = "123#456";
        try {
            Integer.parseInt(str);
            System.out.println("=====See:ERROR!!!");
        } catch (ClassCastException e) {
            System.out.println("=====See:ERROR!!!");
            result1 = 3;
        } catch (NumberFormatException e) {
            result1 = 1;
        } catch (IllegalArgumentException e) {
            System.out.println("=====See:ERROR!!!");
            result1 = 3;
        } finally {
            processResult--;
            try {
                System.out.println(str.substring(-5));
            } catch (NumberFormatException e) {
                processResult -= 10;
            }
        }
        processResult -= 10;
        return result1;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n