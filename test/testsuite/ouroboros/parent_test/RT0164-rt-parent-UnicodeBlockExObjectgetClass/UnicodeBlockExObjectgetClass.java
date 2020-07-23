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
 * -@TestCaseID: UnicodeBlockExObjectgetClass.java
 * -@TestCaseName: Exception in Character/UnicodeBlock:  final Class<?> getClass()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:Test UnicodeBlock api getClass extends from Object
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: UnicodeBlockExObjectgetClass.java
 * -@ExecuteClass: UnicodeBlockExObjectgetClass
 * -@ExecuteArgs:
 */

import java.lang.Character;

public class UnicodeBlockExObjectgetClass {
    static int res = 99;

    public static void main(String argv[]) {
        System.out.println(new UnicodeBlockExObjectgetClass().run());
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = unicodeBlockExObjectgetClass1();
        } catch (Exception e) {
            UnicodeBlockExObjectgetClass.res = UnicodeBlockExObjectgetClass.res - 20;
        }

        if (result == 4 && UnicodeBlockExObjectgetClass.res == 89) {
            result = 0;
        }

        return result;
    }


    private int unicodeBlockExObjectgetClass1() {
        int result1 = 4; /*STATUS_FAILED*/
        // final Class<?> getClass()
        Character.UnicodeBlock unB1 = null;
        for (int cp = 0; cp <= 10; ++cp) {
            unB1 = Character.UnicodeBlock.of(cp);
        }

        Class px1 = unB1.getClass();

        if (px1.toString().equals("class java.lang.Character$UnicodeBlock")) {
            UnicodeBlockExObjectgetClass.res = UnicodeBlockExObjectgetClass.res - 10;
        }
        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n