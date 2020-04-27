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
 * -@TestCaseID: TryCatchReturnExceptionTest.java
 * -@TestCaseName: Test try{exception return_x}-catch（e){return_x}-finally{return_v}.
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: Create a String s include number and special symbol.
 * -#step2: Test static int parseInt(String s).
 * -#step3: Check NumberFormatException is caught by catch.
 * -#step4: Return value in the catch.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: TryCatchReturnExceptionTest.java
 * -@ExecuteClass: TryCatchReturnExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class TryCatchReturnExceptionTest {
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
            result = tryCatchReturnExceptionTest1();
        } catch (NumberFormatException e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 95) {
            result = 0;
        }
        return result;
    }

    /**
     * Test try{exception return_x}-catch（e){return_x}-finally{return_v}.
     *
     * @return status code
     */
    public static int tryCatchReturnExceptionTest1() {
        int result1 = 4; /* STATUS_FAILED */
        String str = "123#456";
        try {
            Integer.parseInt(str);
            return processResult;
        } catch (ClassCastException e) {
            System.out.println("=====See:ERROR!!!");
            result1 = 3;
        } catch (NumberFormatException e) {
            processResult -= 1;
            result1 = 1;
            return result1;
        } catch (IllegalStateException e) {
            System.out.println("=====See:ERROR!!!");
            result1 = 3;
        } finally {
            processResult -= 3;
            result1++;
            return result1; // Execute return 2
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n