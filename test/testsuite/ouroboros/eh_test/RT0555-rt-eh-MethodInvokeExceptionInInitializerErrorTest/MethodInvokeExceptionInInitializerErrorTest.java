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
 * -@TestCaseID: MethodInvokeExceptionInInitializerErrorTest.java
 * -@TestCaseName: Exception in reflect method : public Object invoke(Object obj, Object... args).
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: Create self class has fault static, create class instance by self class construct.
 * -#step2: Get class method from self class.
 * -#step3: Test invoke(Object obj, Object... args), args is a new object Array.
 * -#step4: Check that if ExceptionInInitializerError exception occurs.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: MethodInvokeExceptionInInitializerErrorTest.java
 * -@ExecuteClass: MethodInvokeExceptionInInitializerErrorTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class MethodInvokeExceptionInInitializerErrorTest {
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
        int result = 2; /*STATUS_FAILED*/

        try {
            result = methodInvokeExceptionInInitializerError();
        } catch (Exception e) {
            processResult -= 20;
        }

        if (result == 4 && processResult == 98) {
            result = 0;
        }

        return result;
    }

    /**
     * Exception in reflect method : public Object invoke(Object obj, Object... args).
     *
     * @return status code
     * @throws NoSuchMethodException
     * @throws SecurityException
     * @throws IllegalAccessException
     * @throws IllegalArgumentException
     * @throws InvocationTargetException
     */
    public static int methodInvokeExceptionInInitializerError()
            throws NoSuchMethodException, SecurityException, IllegalAccessException, IllegalArgumentException,
            InvocationTargetException {
        int result1 = 4; /*STATUS_FAILED*/
        Class<TestExceptionInInitializerError> class1 = TestExceptionInInitializerError.class;
        Method method = class1.getMethod("setInt", new Class[]{int.class});
        try {
            method.invoke(new TestExceptionInInitializerError(10), new Object[]{20});
            processResult -= 10;
        } catch (ExceptionInInitializerError e1) {
            processResult--;
        }

        return result1;
    }
}

class TestExceptionInInitializerError {
    int nu;

    /**
     * set num
     *
     * @param num for test
     */
    public static void setInt(int num) {
        int num1 = num;
    }

    public TestExceptionInInitializerError(int num) {
        this.nu = num;
    }

    static {
        int num = 1 / 0;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n