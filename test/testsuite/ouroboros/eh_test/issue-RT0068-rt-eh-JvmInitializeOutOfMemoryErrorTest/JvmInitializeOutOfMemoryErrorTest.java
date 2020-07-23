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
 * -@TestCaseID: JvmInitializeOutOfMemoryErrorTest.java
 * -@TestCaseName: Test jvm instr: invoke virtual thrown Exception InInitializerError.
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: Initialize a big int param, beyond 512M.
 * -#step2: Check OutOfMemoryError is thrown.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: JvmInitializeOutOfMemoryErrorTest.java
 * -@ExecuteClass: JvmInitializeOutOfMemoryErrorTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class JvmInitializeOutOfMemoryErrorTest {
    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public static int run(String[] argv, PrintStream out) {
        JvmInitializeOutOfMemoryErrorTest tTest = new JvmInitializeOutOfMemoryErrorTest();
        return tTest.test();
    }

    /**
     * Test jvm instr: invoke virtual thrown Exception InInitializerError.
     *
     * @return status code
     */
    public static int test() {
        try {
            int[] num;
            num = new int[1024 * 1024 * 1280]; // 1024*1024*4B = 4M, *128=512M
            num[0] = 1; // If no this, y = new int[] will be optimizated and deleted.
        } catch (OutOfMemoryError e2) {
            return 0;
        }
        return 2;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n