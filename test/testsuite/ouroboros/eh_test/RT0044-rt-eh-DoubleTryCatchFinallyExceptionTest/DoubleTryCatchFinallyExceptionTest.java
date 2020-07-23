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
 * -@TestCaseID: DoubleTryCatchFinallyExceptionTest.java
 * -@TestCaseName: Test try{try{exception} catch(x)}-finally -->caller.
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: Create a String s include number and special symbol.
 * -#step2: Test static int parseInt(String s) in try-finally.
 * -#step3: Check NumberFormatException is thrown in outside try-Multi catch-finally, finally is walked.
 * -#step4: Check NumberFormatException is thrown, outside finally is walked.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: DoubleTryCatchFinallyExceptionTest.java
 * -@ExecuteClass: DoubleTryCatchFinallyExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class DoubleTryCatchFinallyExceptionTest {
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
            result = doubleTryCatchFinallyExceptionTest1();
        } catch (IllegalArgumentException e) {
            processResult -= 2;
        }

        if (result == 2 && processResult == 95) {
            result = 0;
        }
        return result;
    }

    /**
     * Test try{try{exception} catch(x)}-finally -->caller.
     *
     * @return status code
     */
    public static int doubleTryCatchFinallyExceptionTest1() {
        int result1 = 4; /* STATUS_FAILED */
        String str = "123#456";
        try {
            try {
                Integer.parseInt(str);
            } catch (ClassCastException e) {
                result1 = 3;
                processResult -= 10;
            } finally {
                processResult--;
            }
            processResult -= 10;
        } catch (ClassCastException e) {
            System.out.println("=====See:ERROR!!!");
            result1 = 3;
        } catch (IndexOutOfBoundsException e) {
            System.out.println("=====See:ERROR!!!");
            result1 = 3;
        } catch (IllegalStateException e) {
            System.out.println("=====See:ERROR!!!");
            result1 = 3;
        } finally {
            processResult--;
        }
        processResult -= 10;
        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n