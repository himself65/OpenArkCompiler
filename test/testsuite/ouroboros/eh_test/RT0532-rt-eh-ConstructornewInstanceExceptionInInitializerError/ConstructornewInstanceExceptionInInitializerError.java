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
 * -@TestCaseID: ConstructornewInstanceExceptionInInitializerError.java
 * -@TestCaseName: In a static variable initialization exception occurs
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:In a static variable initialization exception occurs
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ConstructornewInstanceExceptionInInitializerError.java
 * -@ExecuteClass: ConstructornewInstanceExceptionInInitializerError
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;

public class ConstructornewInstanceExceptionInInitializerError {
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
            result = constructorNewInstanceExceptionInInitializerError();
        } catch (Exception e) {
            processResult -= 20;
        }

        if (result == 4 && processResult == 98) {
            result = 0;
        }
        //        System.out.println("result: " + result);
        //        System.out.println("ConstructornewInstanceExceptionInInitializerError.res: " +
        // ConstructornewInstanceExceptionInInitializerError.res);
        return result;
    }

    /**
     * In a static variable initialization exception occurs
     *
     * @return status code
     * @throws NoSuchMethodException
     * @throws SecurityException
     * @throws InstantiationException
     * @throws IllegalAccessException
     * @throws IllegalArgumentException
     * @throws InvocationTargetException
     */
    public static int constructorNewInstanceExceptionInInitializerError()
            throws NoSuchMethodException, SecurityException, InstantiationException, IllegalAccessException,
            IllegalArgumentException, InvocationTargetException {
        int result1 = 4; /*STATUS_FAILED*/

        // ExceptionInInitializerError - In a static variable initialization exception occurs
        // public T newInstance(Object... initargs)
        Class<Test01n> class1 = Test01n.class;
        Constructor<Test01n> c1 = class1.getConstructor(new Class[]{});

        try {
            Object file1 = c1.newInstance();
            // System.out.println(file1);
            processResult -= 10;
        } catch (ExceptionInInitializerError e1) {
            processResult -= 1;
        }
        return result1;
    }
}

class Test01n {
    /**
     * test a String val
     */
    public String name = "default";

    public String getName() {
        return name;
    }

    public Test01n() {
        this.name = "init";
    }

    static {
        int num = 1 / 0;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n