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
 * -@TestCaseID: StringBuilderExObjectgetClass.java
 * -@TestCaseName: Exception in StringBuilder:  final Class<?> getClass()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create a StringBuilder object1
 * -#step2: Call getClass() on Object1
 * -#step3: Confirm that the returned Class is correct
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: StringBuilderExObjectgetClass.java
 * -@ExecuteClass: StringBuilderExObjectgetClass
 * -@ExecuteArgs:
 */

import java.lang.String;

public class StringBuilderExObjectgetClass {
    static int res = 99;
    private String[] stringArray = {
            "", "a", "b", "c", "ab", "ac", "abc", "aaaabbbccc"
    };

    public static void main(String argv[]) {
        System.out.println(new StringBuilderExObjectgetClass().run());
    }

    /**
     * main test fun
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = stringBuilderExObjectgetClass1();
        } catch (Exception e) {
            StringBuilderExObjectgetClass.res = StringBuilderExObjectgetClass.res - 20;
        }

        if (result == 4 && StringBuilderExObjectgetClass.res == 89) {
            result = 0;
        }

        return result;
    }


    private int stringBuilderExObjectgetClass1() {
        int result1 = 4; /*STATUS_FAILED*/
        //  final Class<?> getClass()

        StringBuilder sb = null;
        for (int i = 0; i < stringArray.length; i++) {
            sb = new StringBuilder(stringArray[i]);
        }
        Class px1 = sb.getClass();

        if (px1.toString().equals("class java.lang.StringBuilder")) {
            StringBuilderExObjectgetClass.res = StringBuilderExObjectgetClass.res - 10;
        }
        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n