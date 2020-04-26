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
 * -@TestCaseID: StringBufferExObjectgetClass.java
 * -@TestCaseName: Exception in String:  final Class<?> getClass()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create a StringBuffer object1
 * -#step2: Call getClass() on Object1
 * -#step3: Confirm that the returned Class is correct
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: StringBufferExObjectgetClass.java
 * -@ExecuteClass: StringBufferExObjectgetClass
 * -@ExecuteArgs:
 */

import java.lang.Class;

public class StringBufferExObjectgetClass {
    static int res = 99;
    private String[] stringArray = {
            "", "a", "b", "c", "ab", "ac", "abc", "aaaabbbccc"
    };

    public static void main(String argv[]) {
        System.out.println(new StringBufferExObjectgetClass().run());
    }

    /**
     * main test fun
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = stringBufferExObjectgetClass1();
        } catch (Exception e) {
            StringBufferExObjectgetClass.res = StringBufferExObjectgetClass.res - 20;
        }

        if (result == 4 && StringBufferExObjectgetClass.res == 89) {
            result = 0;
        }

        return result;
    }


    private int stringBufferExObjectgetClass1() {
        int result1 = 4; /*STATUS_FAILED*/
        //  final Class<?> getClass()

        StringBuffer sb = null;
        for (int i = 0; i < stringArray.length; i++) {
            sb = new StringBuffer(stringArray[i]);
        }
        Class px1 = sb.getClass();
        if (px1.toString().equals("class java.lang.StringBuffer")) {
            StringBufferExObjectgetClass.res = StringBufferExObjectgetClass.res - 10;
        }
        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n