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
 * -@TestCaseID: ConstructorgAnnotationNullPointerException.java
 * -@TestCaseName:Exception in Constructor:public T getAnnotation
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: getAnnotation(Class<T> annotationClass),annotationClass is null
 * -#step2: catch NPE
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ConstructorgAnnotationNullPointerException.java
 * -@ExecuteClass: ConstructorgAnnotationNullPointerException
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.lang.reflect.Constructor;

public class ConstructorgAnnotationNullPointerException {
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
            result = constructorGAnnotationNullPointerException();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 4 && processResult == 98) {
            result = 0;
        }

        return result;
    }

    /**
     * Exception in Constructor:public T getAnnotation
     *
     * @return status code
     * @throws NoSuchMethodException
     * @throws SecurityException
     */
    public static int constructorGAnnotationNullPointerException() throws NoSuchMethodException, SecurityException {
        int result1 = 4; /*STATUS_FAILED*/

        // NullPointerException - Null pointer exception
        // public T getAnnotation(Class<T> annotationClass)
        Class<Test01b> class1 = Test01b.class;
        Constructor<Test01b> c1 = class1.getConstructor(new Class[]{String.class});
        //        Constructor m = Class.class.getMethod("getName", new Class[] {});

        try {
            Object file1 = c1.getAnnotation(null);
            processResult -= 10;
        } catch (NullPointerException e1) {
            processResult -= 1;
        }
        return result1;
    }
}

class Test01b {
    /**
     * just for test
     */
    public String name = "default";

    public String getName() {
        return name;
    }

    public Test01b(String hh) {
        this.name = hh;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n