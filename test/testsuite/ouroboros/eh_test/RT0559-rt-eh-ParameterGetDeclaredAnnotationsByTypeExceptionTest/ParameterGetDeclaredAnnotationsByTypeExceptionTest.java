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
 * -@TestCaseID: ParameterGetDeclaredAnnotationsByTypeExceptionTest.java
 * -@TestCaseName: Exception in Parameter: public T[] getDeclaredAnnotationsByType(Class<T> annotationClass).
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: Create method instance by self class.
 * -#step2: Get method parameters by getParameters() from method instance.
 * -#step3: Test method getDeclaredAnnotationsByType(Class<T> annotationClass), parameter annotationClass is null.
 * -#step4: Check that if NullPointerException exception occurs.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ParameterGetDeclaredAnnotationsByTypeExceptionTest.java
 * -@ExecuteClass: ParameterGetDeclaredAnnotationsByTypeExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.lang.reflect.Method;
import java.lang.reflect.Parameter;

public class ParameterGetDeclaredAnnotationsByTypeExceptionTest {
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
            result = parameterGetDeclaredAnnotationsByTypeException();
        } catch (Exception e) {
            processResult -= 20;
        }

        if (result == 4 && processResult == 98) {
            result = 0;
        }
        return result;
    }

    /**
     * Exception in Parameter: public T[] getDeclaredAnnotationsByType(Class<T> annotationClass).
     *
     * @return status code
     * @throws NoSuchMethodException
     * @throws SecurityException
     */
    public static int parameterGetDeclaredAnnotationsByTypeException()
            throws NoSuchMethodException, SecurityException {
        int result1 = 4; /* STATUS_FAILED */

        Method method = Test06a.class.getMethod("getName", new Class[]{String.class});
        Parameter[] parameters = method.getParameters();
        try {
            Object value = parameters[0].getDeclaredAnnotationsByType(null);
            processResult -= 10;
        } catch (NullPointerException e1) {
            processResult--;
        }
        return result1;
    }
}

class Test06a {
    private String name = "default";

    public String getName(String str1) {
        return str1;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n