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
 * -@TestCaseID:NativeTryCatchNewExceptionTest.java
 * -@TestCaseName: Asynchronous: Native(exception at callback from Java-throw new), captured by Java(try-catch).
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: Declare a native method that throws an exception, Create the native method throws new exception in C file.
 * -#step2: The C file Method will Call java method callback.
 * -#step3: Invoke this native method.
 * -#step4: Check new Exception StringIndexOutOfBoundsException is thrown by Java(try-catch).
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source:NativeTryCatchNewExceptionTest.java jniNativeTryCatchNewExceptionTest.cpp
 * -@ExecuteClass: NativeTryCatchNewExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class NativeTryCatchNewExceptionTest extends Exception {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.exit(run(argv, System.out));
    }

    private native void nativeNativeTryCatchNewExceptionTest() throws IllegalArgumentException;

    private void callback() throws NumberFormatException {
        throw new NumberFormatException("JavaMethodThrows");
    }

    /**
     * main test fun
     * @return status code
     */
    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_FAILED */
        try {
            result = nativeTryCatchNewExceptionTest();
        } catch (Exception e) {
            processResult--;
        }

        if (result == 2 && processResult == 95) {
            result = 0;
        }
        return result;
    }

    /**
     * Asynchronous: Native(exception at callback from Java-throw new), captured by Java(try-catch).
     * @return status code
     */
    public static int nativeTryCatchNewExceptionTest() {
        int result1 = 4; /* STATUS_FAILED */

        NativeTryCatchNewExceptionTest cTest = new NativeTryCatchNewExceptionTest();
        try {
            cTest.nativeNativeTryCatchNewExceptionTest();
            processResult -= 10;
        } catch (NumberFormatException e1) {
            processResult -= 10;
        } catch (IllegalStateException e2) {
            processResult -= 10;
        } finally {
            processResult -= 3;
        }
        return result1;
    }
}

// DEPENDENCE: jniNativeTryCatchNewExceptionTest.cpp
// EXEC:%maple  NativeTryCatchNewExceptionTest.java jniNativeTryCatchNewExceptionTest.cpp %build_option -o %n.so
// EXEC:%run %n.so NativeTryCatchNewExceptionTest  %run_option | compare %f
// ASSERT: scan \-\-\-\-\-\-\>CheckPoint\:CcanContinue