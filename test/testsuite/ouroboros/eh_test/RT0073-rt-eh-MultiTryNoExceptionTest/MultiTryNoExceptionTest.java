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
 * -@TestCaseID: MultiTryNoExceptionTest.java
 * -@TestCaseName: Test no Exception in try-multi catch-finally.
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: Create a String s include number in double try-multi catch-finally.
 * -#step2: Test static int parseInt(String s) in double try-multi catch-finally.
 * -#step3: Check method is invoke correctly, no Exception is thrown.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: MultiTryNoExceptionTest.java
 * -@ExecuteClass: MultiTryNoExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class MultiTryNoExceptionTest {
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
            result = multiTryNoExceptionTest1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 1 && processResult == 95) {
            result = 0;
        }
        return result;
    }

    /**
     * Test no Exception in try-multi catch-finally.
     *
     * @return status code
     */
    public static int multiTryNoExceptionTest1() {
        int result1 = 4; /* STATUS_FAILED */
        String str = "123456";
        try {
            try {
                Integer.parseInt(str);
            } catch (ClassCastException e) {
                result1 = 3;
            } catch (IllegalStateException e) {
                result1 = 3;
            } catch (IndexOutOfBoundsException e) {
                result1 = 3;
            } finally {
                processResult--;
            }
            processResult--;
        } catch (ClassCastException e) {
            result1 = 3;
        } catch (IllegalStateException e) {
            result1 = 3;
        } catch (IndexOutOfBoundsException e) {
            result1 = 3;
        } finally {
            processResult--;
            result1 = 1;
        }
        processResult--;
        return result1;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n