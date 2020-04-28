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
 * -@TestCaseID: MethodInvokeExceptionTest.java
 * -@TestCaseName: Test Exception in invoke of method.
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#case1
 * -#step1: Create a method from getConstructor of java.lang.Class.
 * -#step2: Generate an instance from java.security.CodeSigner. Assign parameters is Class[].
 * -#step3: Test Object invoke(Object obj, Object... args), check exception is thrown correctly.
 * -#case2
 * -#step1: Create a method from CharConversionException of java.lang.Character.
 * -#step2: Generate an instance by class java.security.CodeSigner. Assign parameters is Class[].
 * -#step3: Test Object invoke(Object obj, Object... args), check exception is thrown correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: MethodInvokeExceptionTest.java
 * -@ExecuteClass: MethodInvokeExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.lang.reflect.Method;

public class MethodInvokeExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    private static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_FAILED */

        try {
            invokeMethod();
        } catch (Exception e) {
            processResult--;
            System.out.println(3);
        }

        try {
            invokeMethodTest();
        } catch (Exception e) {
            processResult--;
            System.out.println(6);
        }

        if (result == 2 && processResult == 97) {
            result = 0;
        }
        return result;
    }

    static void invokeMethod() throws Exception {
        Method method = Class.forName("java.lang.Class").getDeclaredMethod("getConstructor", Class[].class);
        Object instance = Class.forName("java.security.CodeSigner");
        Object[] parameters = new Object[1];
        parameters[0] =
                new Class[]{
                        java.security.CodeSigner.class,
                        java.util.concurrent.locks.AbstractOwnableSynchronizer.class,
                        java.util.concurrent.locks.AbstractOwnableSynchronizer.class,
                        java.util.LinkedHashMap.class,
                        java.nio.channels.AsynchronousFileChannel.class,
                        java.sql.Time.class,
                        java.util.EnumMap.class,
                        java.nio.channels.FileLock.class,
                        java.util.WeakHashMap.class
                };
        System.out.println(1);
        method.invoke(instance, parameters);
        System.out.println(2);
    }

    static void invokeMethodTest() throws Exception {
        Method method =
                Class.forName("java.lang.Character").getDeclaredMethod("CharConversionException", Class[].class);
        Object instance = Class.forName("java.security.CodeSigner");
        Object[] parameters = new Object[1];

        parameters[0] =
                new Class[]{
                        java.security.CodeSigner.class,
                        java.util.concurrent.locks.AbstractOwnableSynchronizer.class,
                        java.util.concurrent.locks.AbstractOwnableSynchronizer.class,
                        java.util.LinkedHashMap.class,
                        java.nio.channels.AsynchronousFileChannel.class,
                        java.sql.Time.class,
                        java.util.EnumMap.class,
                        java.nio.channels.FileLock.class,
                        java.util.WeakHashMap.class
                };
        System.out.println(4);
        method.invoke(instance, parameters);
        System.out.println(5);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 1\s*3\s*6\s*0
