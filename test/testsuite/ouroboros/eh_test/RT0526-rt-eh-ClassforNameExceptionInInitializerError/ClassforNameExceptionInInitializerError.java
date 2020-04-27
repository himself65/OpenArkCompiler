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
 * -@TestCaseID: ClassforNameExceptionInInitializerError.java
 * -@TestCaseName: Exception in Class :public static Class<?> forName(String name, boolean initialize, ClassLoader loader)
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1:Class contain RuntimeEH in its static block
 * -#step2:Init by Class.forName,occur InitializerError
 * -#step3:catch EH
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ClassforNameExceptionInInitializerError.java
 * -@ExecuteClass: ClassforNameExceptionInInitializerError
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class ClassforNameExceptionInInitializerError {
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
            result = classForNameExceptionInInitializerError();
        } catch (Exception e) {
            processResult -= 20;
        }

        if (result == 4 && processResult == 98) {
            result = 0;
        }

        return result;
    }

    /**
     * Exception in Class :public static Class<?> forName(String name, boolean initialize, ClassLoader loader)
     *
     * @return status code
     * @throws ClassNotFoundException
     */
    public static int classForNameExceptionInInitializerError() throws ClassNotFoundException {
        int result1 = 4; /*STATUS_FAILED*/

        // ExceptionInInitializerError - if the initialization provoked by this method fails
        //
        // public static Class<?> forName(String name, boolean initialize, ClassLoader loader)
        Class cls = Class.forName("ClassforNameExceptionInInitializerError");
        ClassLoader cLoader = cls.getClassLoader();

        try {
            Class cls2 = Class.forName("Cast5", true, cLoader);
            processResult -= 10;
        } catch (ExceptionInInitializerError e1) {
            processResult--;
        }

        return result1;
    }
}

class Cast5 {
    String str1 = "str1";

    /**
     * set String method1
     */
    public static void setStr() {
        String method1 = "1";
    }

    static {
        int num = 1 / 0;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n