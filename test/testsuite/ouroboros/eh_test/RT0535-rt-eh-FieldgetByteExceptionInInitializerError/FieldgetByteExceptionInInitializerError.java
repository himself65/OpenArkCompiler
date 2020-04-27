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
 * -@TestCaseID: FieldgetByteExceptionInInitializerError.java
 * -@TestCaseName: Exception in reflect filed:public byte getByte(Object obj)
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1:prepare Class with wrong init order
 * -#step2:trigger class init by Field.getByte
 * -#step3:catch EH
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: FieldgetByteExceptionInInitializerError.java
 * -@ExecuteClass: FieldgetByteExceptionInInitializerError
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.lang.reflect.Field;

public class FieldgetByteExceptionInInitializerError {
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
            result = fieldGetByteExceptionInInitializerError();
        } catch (Exception e) {
            processResult -= 20;
        }

        if (result == 4 && processResult == 98) {
            result = 0;
        }

        return result;
    }

    /**
     * Exception in reflect filed:public byte getByte(Object obj)
     *
     * @return status code
     * @throws NoSuchFieldException
     * @throws SecurityException
     * @throws IllegalArgumentException
     * @throws IllegalAccessException
     */
    public static int fieldGetByteExceptionInInitializerError()
            throws NoSuchFieldException, SecurityException, IllegalArgumentException, IllegalAccessException {
        int result1 = 4; /*STATUS_FAILED*/

        // ExceptionInInitializerError -if the initialization provoked by this method fails.
        //
        // public byte getByte(Object obj)
        Field field = Test03aa.class.getDeclaredField("field6");
        try {
            byte obj = field.getByte(new Test03aa());
            processResult -= 10;
        } catch (ExceptionInInitializerError e1) {
            processResult--;
        }

        return result1;
    }
}

class Test03aa {
    /**
     * define a byte for test
     */
    public static byte field6 = selfByte();
    /**
     * define a int[] filed for test
     */
    public static int[] field2 = {1, 2, 3, 4};

    /**
     * set bValue and return
     *
     * @return bValue
     */
    public static byte selfByte() {
        int self1 = field2[2];
        byte bValue = 11;
        return bValue;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n