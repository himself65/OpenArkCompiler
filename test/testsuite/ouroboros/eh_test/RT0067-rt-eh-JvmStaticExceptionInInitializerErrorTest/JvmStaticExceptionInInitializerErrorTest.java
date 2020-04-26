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
 * -@TestCaseID: JvmStaticExceptionInInitializerErrorTest.java
 * -@TestCaseName: Test jvm instr: invoke static thrown ExceptionInInitializerError.
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: Create a static variable that initialize will be fault of a class.
 * -#step2: Test static method of the class.
 * -#step3: Check ExceptionInInitializerError is caught by catch.
 * -#step4: Try test static method of the class.
 * -#step5: Check NoClassDefFoundError is caught by catch.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: JvmStaticExceptionInInitializerErrorTest.java
 * -@ExecuteClass: JvmStaticExceptionInInitializerErrorTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

class JvmStaticExceptionInInitializerErrorTesta {
    static int other_class_test_field = 21;
    static int local_field = 5 / 0;

    static int testFail() {
        int aNum = 1;
        return aNum;
    }
}

public class JvmStaticExceptionInInitializerErrorTest {
    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public static int run(String[] argv, PrintStream out) {
        try {
            int tmp = JvmStaticExceptionInInitializerErrorTesta.testFail();
        } catch (ExceptionInInitializerError e1) {
            try {
                int tmp = JvmStaticExceptionInInitializerErrorTesta.testFail();
            } catch (NoClassDefFoundError e2) {
                return 0; // STATUS_PASSED
            }
        }
        return 2; // STATUS_FAILED
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n