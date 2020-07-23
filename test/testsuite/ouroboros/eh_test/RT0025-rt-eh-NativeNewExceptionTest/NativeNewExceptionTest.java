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
 * -@TestCaseID: NativeNewExceptionTest.java
 * -@TestCaseName: Asynchronous: Native(exception-throw new), captured by Java.
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: Declare a native method that throws an exception, Create the native method throws new exception in C file.
 * -#step2: The C file Method will Call java method callback.
 * -#step3: Invoke this native method.
 * -#step4: Check new Exception StringIndexOutOfBoundsException is thrown.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: NativeNewExceptionTest.java jniNativeNewExceptionTest.cpp
 * -@ExecuteClass: NativeNewExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class NativeNewExceptionTest extends Exception {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.exit(run(argv, System.out));
    }

    private native void nativeNativeNewExceptionTest() throws IllegalStateException;

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
            result = nativeNewExceptionTest();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 3 && processResult == 95) {
            result = 0;
        }

        return result;
    }

    /**
     * Asynchronous: Native(exception-throw new), captured by Java.
     * @return status code
     */
    public static int nativeNewExceptionTest() {
        int result1 = 4; /* STATUS_FAILED */

        NativeNewExceptionTest cTest = new NativeNewExceptionTest();
        try {
            cTest.nativeNativeNewExceptionTest();
            processResult -= 10;
        } catch (NumberFormatException e1) {
            processResult -= 10;
        } catch (IllegalStateException e2) {
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e3) {
            result1 = 3;
            processResult--;
        }
        processResult -= 3;
        return result1;
    }
}

// DEPENDENCE: jniNativeNewExceptionTest.cpp
// EXEC:%maple  NativeNewExceptionTest.java jniNativeNewExceptionTest.cpp %build_option -o %n.so
// EXEC:%run %n.so NativeNewExceptionTest  %run_option | compare %f
// ASSERT: scan-full ------>CheckPoint:CcanContinue\n