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
 * -@TestCaseID: NativeCodeCatchThrowNewExceptionTest.java
 * -@TestCaseName: Asynchronous: Native(exception at callback from Java, then check-throw new), captured by Java.
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: Declare a native method that throws an exception e1.
 * -#step2: Invoke this native method.
 * -#step3: The exception e1 is captured by C code, when e1 get, then new Exception e2 is thrown.
 * -#step4: Check new Exception e2 is captured by java(try/catch).
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: NativeCodeCatchThrowNewExceptionTest.java jniNativeCodeCatchThrowNewExceptionTest.cpp
 * -@ExecuteClass: NativeCodeCatchThrowNewExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.nio.file.InvalidPathException;

public class NativeCodeCatchThrowNewExceptionTest extends Exception {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.exit(run(argv, System.out));
    }

    private native void nativeNativeCodeCatchThrowNewExceptionTest() throws InvalidPathException;

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
            result = nativeCodeCatchThrowNewExceptionTest();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 1 && processResult == 95) {
            result = 0;
        }
        return result;
    }

    /**
     *  Asynchronous: Native(exception at callback from Java, then check-throw new), captured by Java.
     * @return status code
     */
    public static int nativeCodeCatchThrowNewExceptionTest() {
        int result1 = 4; /* STATUS_FAILED */

        NativeCodeCatchThrowNewExceptionTest cTest = new NativeCodeCatchThrowNewExceptionTest();
        try {
            cTest.nativeNativeCodeCatchThrowNewExceptionTest();
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

// DEPENDENCE: jniNativeCodeCatchThrowNewExceptionTest.cpp
// EXEC:%maple  NativeCodeCatchThrowNewExceptionTest.java jniNativeCodeCatchThrowNewExceptionTest.cpp %build_option -o %n.so
// EXEC:%run %n.so NativeCodeCatchThrowNewExceptionTest  %run_option | compare %f
// ASSERT: scan-full ------>CheckPoint:CcanContinue\n