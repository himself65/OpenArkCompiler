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
 * -@TestCaseID: FieldgetExceptionInInitializerError.java
 * -@TestCaseName: Exception in reflect field:public Object get(Object obj)
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1:prepare Class with RuntimeEH
 * -#step2:trigger class init by invoke Field
 * -#step3:catch EH
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: FieldgetExceptionInInitializerError.java
 * -@ExecuteClass: FieldgetExceptionInInitializerError
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.lang.reflect.Field;

public class FieldgetExceptionInInitializerError {
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
            result = fieldGetExceptionInInitializerError();
        } catch (Exception e) {
            processResult -= 20;
        }

        if (result == 4 && processResult == 98) {
            result = 0;
        }

        return result;
    }

    /**
     * Exception in reflect field:public Object get(Object obj)
     *
     * @return status code
     * @throws ClassNotFoundException
     * @throws NoSuchFieldException
     * @throws SecurityException
     * @throws IllegalArgumentException
     * @throws IllegalAccessException
     */
    public static int fieldGetExceptionInInitializerError()
            throws ClassNotFoundException, NoSuchFieldException, SecurityException, IllegalArgumentException,
            IllegalAccessException {
        int result1 = 4; /*STATUS_FAILED*/

        // ExceptionInInitializerError -if the initialization provoked by this method fails.
        //
        // public Object get(Object obj)
        //        Test01aa class1 = new Test01aa();
        Field field = Test01aa.class.getDeclaredField("field1");
        try {
            Object obj = field.get(new Test01aa());
            processResult -= 10;
        } catch (ExceptionInInitializerError e1) {
            processResult--;
        }

        return result1;
    }
}

class Test01aa {
    /**
     * a int field for test
     */
    public int field1 = 1;
    /**
     * a int[] field for test
     */
    public static int[] field2 = {1, 2, 3, 4};
    /**
     * a int field for test
     */
    public static int field3 = field2[3];

    static {
        int field4 = 1 / 0;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n