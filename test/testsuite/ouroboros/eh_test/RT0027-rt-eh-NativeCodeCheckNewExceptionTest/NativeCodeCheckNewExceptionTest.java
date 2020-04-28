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
 * -@TestCaseID: NativeCodeCheckNewExceptionTest.java
 * -@TestCaseName: Asynchronous: Native(exception at callback from Java, then Occurred-throw new), catch by Java.
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: Declare a native method that throws an exception, Create the native method throws new exception in C file.
 * -#step2: The C file Method will Call java method callback, use ExceptionOccurred check Exception.
 * -#step3: Invoke this native method.
 * -#step4: Check new Exception StringIndexOutOfBoundsException is thrown.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: NativeCodeCheckNewExceptionTest.java jniNativeCodeCheckNewExceptionTest.cpp
 * -@ExecuteClass: NativeCodeCheckNewExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.nio.file.InvalidPathException;

public class NativeCodeCheckNewExceptionTest extends Exception {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.exit(run(argv, System.out));
    }

    private native void nativeNativeCodeCheckNewExceptionTest() throws InvalidPathException;

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
            result = nativeCodeCheckNewExceptionTest();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 1 && processResult == 95) {
            result = 0;
        }

        return result;
    }

    /**
     * Asynchronous: Native(exception at callback from Java, then Occurred-throw new), catch by Java.
     * @return status code
     */
    public static int nativeCodeCheckNewExceptionTest() {
        int result1 = 4; /* STATUS_FAILED */

        NativeCodeCheckNewExceptionTest cTest = new NativeCodeCheckNewExceptionTest();
        try {
            cTest.nativeNativeCodeCheckNewExceptionTest();
            processResult -= 10;
        } catch (NumberFormatException e1) {
            result1 = 3;
            System.out.println("======>" + e1.getMessage());
            processResult -= 10;
        } catch (InvalidPathException e2) {
            result1 = 3;
            System.out.println("======>" + e2.getMessage());
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e3) {
            result1 = 1;
            processResult--;
        }
        processResult -= 3;
        return result1;
    }
}

// DEPENDENCE: jniNativeCodeCheckNewExceptionTest.cpp
// EXEC:%maple  NativeCodeCheckNewExceptionTest.java jniNativeCodeCheckNewExceptionTest.cpp %build_option -o %n.so
// EXEC:%run %n.so NativeCodeCheckNewExceptionTest  %run_option | compare %f
// ASSERT: scan \-\-\-\-\-\-\>CheckPoint\:CcanContinue