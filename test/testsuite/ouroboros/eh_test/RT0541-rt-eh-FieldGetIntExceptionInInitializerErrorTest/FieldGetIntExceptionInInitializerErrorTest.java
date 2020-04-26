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
 * -@TestCaseID: FieldGetIntExceptionInInitializerErrorTest.java
 * -@TestCaseName: Exception in reflect filed: public int getInt(Object obj).
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: Create a TestInt class, and create a public static selfDouble method and static area has expression
 *          error.
 * -#step2: Create a Field instance f by calling the getDeclareFiled method on the TestInt object.
 * -#step3: Test method getInt(Object obj).
 * -#step4：Check ExceptionInInitializerError is thrown correctly.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: FieldGetIntExceptionInInitializerErrorTest.java
 * -@ExecuteClass: FieldGetIntExceptionInInitializerErrorTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.lang.reflect.Field;

public class FieldGetIntExceptionInInitializerErrorTest {
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
            result = fieldGetIntExceptionInInitializerErrorTest();
        } catch (Exception e) {
            processResult -= 20;
        }

        if (result == 4 && processResult == 98) {
            result = 0;
        }
        return result;
    }

    /**
     * Exception in reflect filed: public int getInt(Object obj).
     *
     * @return status code
     * @throws NoSuchFieldException
     * @throws SecurityException
     * @throws IllegalArgumentException
     * @throws IllegalAccessException
     */
    public static int fieldGetIntExceptionInInitializerErrorTest()
            throws NoSuchFieldException, SecurityException, IllegalArgumentException, IllegalAccessException {
        int result1 = 4; /* STATUS_FAILED */

        Field field = TestInt.class.getDeclaredField("field6");
        try {
            int obj = field.getInt(new TestInt());
            processResult -= 10;
        } catch (ExceptionInInitializerError e1) {
            processResult--;
        }
        return result1;
    }
}

class TestInt {
    /**
     * a int field for test
     */
    public static int field6 = selfDouble();
    /**
     * a int[] field for test
     */
    public static int[] field2 = {1, 2, 3, 4};

    /**
     * set iValue and return
     *
     * @return iValue
     */
    public static int selfDouble() {
        int self1 = field2[2];
        int iValue = 11;
        return iValue;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n