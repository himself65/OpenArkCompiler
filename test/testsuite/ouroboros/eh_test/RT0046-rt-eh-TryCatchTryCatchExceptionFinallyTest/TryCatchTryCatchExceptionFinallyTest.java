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
 * -@TestCaseID: TryCatchTryCatchExceptionFinallyTest.java
 * -@TestCaseName: Test try{try{exception} catch(x)}-catch(e)-finally.
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: Create a String s include number and special symbol.
 * -#step2: Test static int parseInt(String s) in try-catch.
 * -#step3: Check NumberFormatException is thrown in outside try-Multi catch-finally.
 * -#step4: Check NumberFormatException is caught by catch, outside finally is walked.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: TryCatchTryCatchExceptionFinallyTest.java
 * -@ExecuteClass: TryCatchTryCatchExceptionFinallyTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.util.FormatterClosedException;

public class TryCatchTryCatchExceptionFinallyTest {
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
            result = tryCatchTryCatchExceptionFinallyTest1();
        } catch (NumberFormatException e) {
            processResult -= 10;
        }

        if (result == 1 && processResult == 95) {
            result = 0;
        }
        return result;
    }

    /**
     * Test try{try{exception} catch(x)}-catch(e)-finally.
     *
     * @return status code
     */
    public static int tryCatchTryCatchExceptionFinallyTest1() {
        int result1 = 4; /* STATUS_FAILED */
        String str = "123#456";
        try {
            try {
                Integer.parseInt(str);
            } catch (IllegalStateException e113) {
                System.out.println("=====See:ERROR!!!");
                processResult -= 10;
            }
            processResult -= 10;
        } catch (FormatterClosedException e1132) {
            System.out.println("=====See:ERROR!!!");
            result1 = 3;
        } catch (IllegalArgumentException e111) {
            result1 = 1;
        } catch (IndexOutOfBoundsException e112) {
            System.out.println("=====See:ERROR!!!");
            result1 = 3;
        } finally {
            processResult -= 1;
        }
        processResult -= 3;
        return result1;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n