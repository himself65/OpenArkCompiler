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
 * -@TestCaseID: JvmArrayIndexOutOfBoundsExceptionTest.java
 * -@TestCaseName: Test jvm instr: Invoke Initialization thrown ArrayIndexOutOfBoundsException.
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: Create a object array instance objs.
 * -#step2: Get a object instance by the num element of objs, num is objs.length.
 * -#step3: Check ArrayIndexOutOfBoundsException is thrown correctly.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: JvmArrayIndexOutOfBoundsExceptionTest.java
 * -@ExecuteClass: JvmArrayIndexOutOfBoundsExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class JvmArrayIndexOutOfBoundsExceptionTest {
    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    static final int len = 10;

    /**
     * main test fun
     *
     * @return status code
     */
    public static int run(String[] argv, PrintStream out) {
        Object[] objs = new Object[len];
        try {
            Object obj1 = objs[len];
        } catch (ArrayIndexOutOfBoundsException e) {
            return 0; // STATUS_PASSED
        }
        return 2; // STATUS_FAILED
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n