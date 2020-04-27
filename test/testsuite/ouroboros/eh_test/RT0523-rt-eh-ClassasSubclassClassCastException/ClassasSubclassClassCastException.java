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
 * -@TestCaseID: ClassasSubclassClassCastException.java
 * -@TestCaseName: Exception in Class:public Class<? extends U> asSubclass(Class<U> clazz)
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1:create "subclass" includes the class itself
 * -#step2:this Class object does not represent a subclass of the specified class
 * -#step3:catch exception
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ClassasSubclassClassCastException.java
 * -@ExecuteClass: ClassasSubclassClassCastException
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.util.List;

public class ClassasSubclassClassCastException {
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
            result = classAsSubclassClassCastException();
        } catch (Exception e) {
            processResult -= 20;
        }

        if (result == 4 && processResult == 98) {
            result = 0;
        }
        return result;
    }

    /**
     * asSubclass(Class<U> clazz), check ClassCastException
     *
     * @return status code
     */
    public static int classAsSubclassClassCastException() {
        int result1 = 4; /*STATUS_FAILED*/

        // ClassCastException - if this Class object does not represent a subclass of the specified class
        // (here "subclass" includes the class itself).
        //
        // public Class<? extends U> asSubclass(Class<U> clazz)
        La1 la1 = new La1();
        try {
            Class<? extends List> strListCast = la1.getClass().asSubclass(List.class);
            processResult -= 10;
        } catch (ClassCastException e1) {
            processResult -= 1;
        }

        return result1;
    }
}

class La1 {
    /**
     * just for test
     */
    public int field1 = 1;
    /**
     * just for test
     */
    public static int[] field2 = {1, 2, 3, 4};
    /**
     * just for test
     */
    public static int field3 = field2[3];
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n