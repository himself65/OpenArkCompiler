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
 * -@TestCaseID: ClasscastClassCastException.java
 * -@TestCaseName: Exception in Class:public T cast(Object obj)
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: this Class object does not represent a subclass of the specified class
 * -#step2: catch EH
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ClasscastClassCastException.java
 * -@ExecuteClass: ClasscastClassCastException
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class ClasscastClassCastException {
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
            result = classCastClassCastException();
        } catch (Exception e) {
            processResult -= 20;
        }

        if (result == 4 && processResult == 98) {
            result = 0;
        }

        return result;
    }

    /**
     * Exception in Class:public T cast(Object obj)
     *
     * @return status code
     */
    public static int classCastClassCastException() {
        int result1 = 4; /*STATUS_FAILED*/

        // ClassCastException - if this Class object does not represent a subclass of the specified class
        // (here "subclass" includes the class itself).
        //
        // public T cast(Object obj)
        Object obj1 = new Cast1();
        Cast2 cast22 = new Cast2();
        try {
            Object type1 = Cast1.class.cast(cast22);
            System.out.println(type1.getClass());
            processResult -= 10;
        } catch (ClassCastException e1) {
            processResult -= 1;
        }

        return result1;
    }
}

class Cast1 {
    String str1 = "str1";

    /**
     * set String method1
     */
    public static void setStr() {
        String method1 = "1";
    }
}

class Cast2 {
    String str2 = "str2";

    /**
     * set String method2
     */
    public static void setInt() {
        int method2 = 2;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n