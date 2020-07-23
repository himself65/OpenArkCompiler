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
 * -@TestCaseID: FieldgetDoubleExceptionInInitializerError.java
 * -@TestCaseName: Exception in reflect filed:public double getDouble(Object obj)
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1:prepare Class with wrong init order
 * -#step2:invoke this Class's Field,use Field.getDouble to trigger class init
 * -#step3:catch EH
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: FieldgetDoubleExceptionInInitializerError.java
 * -@ExecuteClass: FieldgetDoubleExceptionInInitializerError
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.lang.reflect.Field;

public class FieldgetDoubleExceptionInInitializerError {
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
            result = fieldGetDoubleExceptionInInitializerError();
        } catch (Exception e) {
            processResult -= 20;
        }

        if (result == 4 && processResult == 98) {
            result = 0;
        }

        return result;
    }

    /**
     * Exception in reflect filed:public double getDouble(Object obj)
     *
     * @return status code
     * @throws NoSuchFieldException
     * @throws SecurityException
     * @throws IllegalArgumentException
     * @throws IllegalAccessException
     */
    public static int fieldGetDoubleExceptionInInitializerError()
            throws NoSuchFieldException, SecurityException, IllegalArgumentException, IllegalAccessException {
        int result1 = 4; /*STATUS_FAILED*/
        // ExceptionInInitializerError -if the initialization provoked by this method fails.
        //
        // public double getDouble(Object obj)
        Field field = TestDouble.class.getDeclaredField("field6");
        try {
            double obj = field.getDouble(new TestDouble());
            processResult -= 10;
        } catch (ExceptionInInitializerError e1) {
            processResult--;
        }

        return result1;
    }
}

class TestDouble {
    /**
     * a double field for test
     */
    public static double field6 = selfDouble();
    /**
     * a int[] field for test
     */
    public static int[] field2 = {1, 2, 3, 4};

    /**
     * set dValue and return
     */
    public static double selfDouble() {
        int self1 = field2[2];
        double dValue = 11;
        return dValue;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n