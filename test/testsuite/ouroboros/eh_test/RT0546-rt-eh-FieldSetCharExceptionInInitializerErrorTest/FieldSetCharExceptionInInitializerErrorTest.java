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
 * -@TestCaseID: FieldSetCharExceptionInInitializerErrorTest.java
 * -@TestCaseName: Exception in reflect filed:public void setChar(Object obj, char c).
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: Create a TestSetChar class, and create a public static selfShort method and static area has expression
 *          error.
 * -#step2: Create a Field instance f by calling the getDeclareFiled method on the TestSetChar object.
 * -#step3: Test method setChar(Object obj, char c).
 * -#step4：Check ExceptionInInitializerError is thrown correctly.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: FieldSetCharExceptionInInitializerErrorTest.java
 * -@ExecuteClass: FieldSetCharExceptionInInitializerErrorTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.lang.reflect.Field;

public class FieldSetCharExceptionInInitializerErrorTest {
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
            result = fieldSetCharExceptionInInitializerErrorTest();
        } catch (Exception e) {
            processResult -= 20;
        }

        if (result == 4 && processResult == 98) {
            result = 0;
        }
        return result;
    }

    /**
     * Exception in reflect filed:public void setChar(Object obj, char c).
     *
     * @return status code
     * @throws ClassNotFoundException
     * @throws NoSuchFieldException
     * @throws SecurityException
     * @throws IllegalArgumentException
     * @throws IllegalAccessException
     */
    public static int fieldSetCharExceptionInInitializerErrorTest()
            throws ClassNotFoundException, NoSuchFieldException, SecurityException, IllegalArgumentException,
            IllegalAccessException {
        int result1 = 4; /* STATUS_FAILED */

        Field field = TestSetChar.class.getDeclaredField("field6");
        try {
            field.setChar(new TestSetChar(), 'c');
            processResult -= 10;
        } catch (ExceptionInInitializerError e1) {
            processResult--;
        }
        return result1;
    }
}

class TestSetChar {
    /**
     * a char field for test
     */
    public static char field6 = selfShort();
    /**
     * a int[] field for test
     */
    public static int[] field2 = {1, 2, 3, 4};

    /**
     * set value and return
     *
     * @return value
     */
    public static char selfShort() {
        int self1 = field2[2];
        char value = 'a';
        return value;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n