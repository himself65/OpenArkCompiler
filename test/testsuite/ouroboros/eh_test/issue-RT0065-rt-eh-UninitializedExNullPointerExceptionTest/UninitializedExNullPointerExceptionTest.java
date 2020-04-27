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
 * -@TestCaseID: UninitializedExNullPointerExceptionTest.java
 * -@TestCaseName: Test Jvm instr: invoke special throw NullPointException.
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: Declare a static class.
 * -#step2: Invoke a method of static class.
 * -#step3: Check NullPointException is thrown.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: UninitializedExNullPointerExceptionTest.java
 * -@ExecuteClass: UninitializedExNullPointerExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class UninitializedExNullPointerExceptionTest {
    static int processResult = 2;
    static UninitializedExNullPointerExceptionTest iv;

    private void proc2() {
    }

    /**
     * Test Jvm instr: invoke special throw NullPointException.
     */
    public static void proc1() {
        try {
            iv.proc2(); // the invoke special instruction is used here
        } catch (NullPointerException e) {
            processResult = 0; // STATUS_PASSED
        }
    }

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public static int run(String[] argv, PrintStream out) {
        proc1();
        return processResult;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n