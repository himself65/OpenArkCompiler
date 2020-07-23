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
 * -@TestCaseID:FieldExObjectgetClass.java
 * -@TestCaseName: Exception in reflect/Field:  final Class<?> getClass()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create class FieldExObjectgetClass, call getDeclaredFields() of class of FieldExObjectgetClass as object1
 * -#step2: Call getClass() on object1[0]
 * -#step3: Confirm that the returned Class is correct
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: FieldExObjectgetClass.java
 * -@ExecuteClass: FieldExObjectgetClass
 * -@ExecuteArgs:
 */


import java.lang.reflect.Field;

public class FieldExObjectgetClass {
    static int res = 99;

    public static void main(String argv[]) {
        System.out.println(new FieldExObjectgetClass().run());
    }

    /**
     * main test fun
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = fieldExObjectgetClass1();
        } catch (Exception e) {
            FieldExObjectgetClass.res = FieldExObjectgetClass.res - 20;
        }

        if (result == 4 && FieldExObjectgetClass.res == 89) {
            result = 0;
        }

        return result;
    }


    private int fieldExObjectgetClass1() {
        //  final Class<?> getClass()
        int result1 = 4; /*STATUS_FAILED*/
        Field[] f1 = FieldExObjectgetClass.class.getDeclaredFields();

        Class px1 = f1[0].getClass();

        if (px1.toString().equals("class java.lang.reflect.Field")) {
            FieldExObjectgetClass.res = FieldExObjectgetClass.res - 10;
        }
        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n