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
 * -@TestCaseID: CharacterSubsetExObjectgetClass.java
 * -@TestCaseName: Exception in Character/Subset:  final Class<?> getClass()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Construct MySubset s by new MySubset (char name)
 * -#step2: Call getClass() on s
 * -#step3: Confirm that the returned object is correct
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: CharacterSubsetExObjectgetClass.java
 * -@ExecuteClass: CharacterSubsetExObjectgetClass
 * -@ExecuteArgs:
 */

import java.lang.Thread;

public class CharacterSubsetExObjectgetClass {
    static int res = 99;

    public static void main(String argv[]) {
        System.out.println(new CharacterSubsetExObjectgetClass().run());
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = characterSubsetExObjectgetClass1();
        } catch (Exception e) {
            CharacterSubsetExObjectgetClass.res = CharacterSubsetExObjectgetClass.res - 20;
        }
        if (result == 4 && CharacterSubsetExObjectgetClass.res == 89) {
            result = 0;
        }

        return result;
    }


    private int characterSubsetExObjectgetClass1() {
//      final Class<?> getClass()
        int result1 = 4; /*STATUS_FAILED*/
        MySubset subset = new MySubset("some subset");
        Class px1 = subset.getClass();
        if (px1.toString().equals("class MySubset")) {
            CharacterSubsetExObjectgetClass.res = CharacterSubsetExObjectgetClass.res - 10;
        }
        return result1;
    }
}

class MySubset extends Character.Subset {
    MySubset(String name) {
        super(name);
    }
}



// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n