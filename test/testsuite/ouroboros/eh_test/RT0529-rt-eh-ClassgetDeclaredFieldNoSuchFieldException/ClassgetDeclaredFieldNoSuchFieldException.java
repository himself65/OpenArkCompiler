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
 * -@TestCaseID: ClassgetDeclaredFieldNoSuchFieldException.java
 * -@TestCaseName: Exception in Class constructor:public Field getDeclaredField(String name)
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:invoke getDeclaredField(String name) without this field name
 * -#step1:invoke getDeclaredField(String name) without this field name
 * -#step2:catch EH
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ClassgetDeclaredFieldNoSuchFieldException.java
 * -@ExecuteClass: ClassgetDeclaredFieldNoSuchFieldException
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.lang.reflect.Field;

public class ClassgetDeclaredFieldNoSuchFieldException {
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
            result = classGetDeclaredFieldNoSuchFieldException();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 4 && processResult == 98) {
            result = 0;
        }

        return result;
    }

    /**
     * Exception in Class constructor:public Field getDeclaredField(String name)
     *
     * @return status code
     */
    public static int classGetDeclaredFieldNoSuchFieldException() {
        int result1 = 4; /*STATUS_FAILED*/

        // NoSuchFieldException - if a field with the specified name is not found.
        //
        // public Field getDeclaredField(String name)
        Class class1 = Class.class;
        try {
            Field file1 = class1.getDeclaredField("abc123");
            processResult -= 10;
        } catch (NoSuchFieldException e1) {
            processResult--;
        }

        return result1;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n