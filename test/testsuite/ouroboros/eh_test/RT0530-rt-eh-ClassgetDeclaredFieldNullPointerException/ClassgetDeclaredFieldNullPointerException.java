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
 * -@TestCaseID: ClassgetDeclaredFieldNullPointerException.java
 * -@TestCaseName: Exception in Class constructor:public Field getDeclaredField(String name)
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1:getDeclaredField(String name),name is null
 * -#step2:catch Exception
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ClassgetDeclaredFieldNullPointerException.java
 * -@ExecuteClass: ClassgetDeclaredFieldNullPointerException
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.lang.reflect.Field;

public class ClassgetDeclaredFieldNullPointerException {
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
            result = classGetDeclaredFieldNullPointerException();
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
     * @throws NoSuchFieldException
     */
    public static int classGetDeclaredFieldNullPointerException() throws NoSuchFieldException {
        int result1 = 4; /*STATUS_FAILED*/

        // NullPointerException -     if name is null.
        //
        // public Field getDeclaredField(String name)

        String string = null;
        Class class1 = Class.class;
        try {
            Field file1 = class1.getDeclaredField(string);
            processResult -= 10;
        } catch (NullPointerException e1) {
            processResult--;
        }
        return result1;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n