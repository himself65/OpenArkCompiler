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
 * -@TestCaseID: ClassgAnnotationsByTypeNullPointerException.java
 * -@TestCaseName:Exception in AccessibleObject:public A[] getAnnotationsByType(Class<A> annotationClass)
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1:annotaitonClass is null,invoke getAnnotationsByType(Class<A> annotationClass)
 * -#step2:catch NPE
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ClassgAnnotationsByTypeNullPointerException.java
 * -@ExecuteClass: ClassgAnnotationsByTypeNullPointerException
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class ClassgAnnotationsByTypeNullPointerException {
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
            result = classGAnnotationsByTypeNullPointerException();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 4 && processResult == 98) {
            result = 0;
        }

        return result;
    }

    /**
     * Exception in AccessibleObject:public A[] getAnnotationsByType(Class<A> annotationClass)
     *
     * @return status code
     * @throws NoSuchMethodException
     * @throws SecurityException
     */
    public static int classGAnnotationsByTypeNullPointerException() throws NoSuchMethodException, SecurityException {
        int result1 = 4; /*STATUS_FAILED*/

        // NullPointerException- Null pointer exception
        // public A[] getAnnotationsByType(Class<A> annotationClass)

        Class class1 = Class.class;

        try {
            Object file1 = class1.getAnnotationsByType(null);
            processResult -= 10;
        } catch (NullPointerException e1) {
            processResult--;
        }
        return result1;
    }
}

class Test01c {
    private String name = "default";

    public String getName() {
        return name;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n