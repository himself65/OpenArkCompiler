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
 * -@TestCaseID: FieldExAccessibleObjectisAccessible.java
 * -@TestCaseName: Exception in reflect/Field:    boolean isAccessible()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create a class SampleClassField_h2, which has a field id, and annotate the class with CustomAnnotations_h2
 * -#step2: Call the run of the use case, and execute method 1. In the method, get the filed "id" by calling
 *          getDeclaredField as Object1
 * -#step3: Call isAccessible() on object 1 to confirm that the returned are correct
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: FieldExAccessibleObjectisAccessible.java
 * -@ExecuteClass: FieldExAccessibleObjectisAccessible
 * -@ExecuteArgs:
 */


import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;
import java.lang.reflect.Field;

public class FieldExAccessibleObjectisAccessible {
    static int res = 99;

    public static void main(String[] argv) {
        System.out.println(new FieldExAccessibleObjectisAccessible().run());
    }

    /**
     * main test fun
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = fieldExAccessibleObjectisAccessible1();
        } catch (Exception e) {
            FieldExAccessibleObjectisAccessible.res = FieldExAccessibleObjectisAccessible.res - 20;
        }

        if (result == 4 && FieldExAccessibleObjectisAccessible.res == 89) {
            result = 0;
        }

        return result;
    }

    private int fieldExAccessibleObjectisAccessible1() throws NoSuchFieldException {
        //  boolean isAccessible()
        int result1 = 4;
        Field f1 = SampleClassFieldH2.class.getDeclaredField("id");
        try {
            if (!f1.isAccessible()) {
                FieldExAccessibleObjectisAccessible.res = FieldExAccessibleObjectisAccessible.res - 10;
            } else {
                FieldExAccessibleObjectisAccessible.res = FieldExAccessibleObjectisAccessible.res - 15;
            }
        } catch (Exception e) {
            FieldExAccessibleObjectisAccessible.res = FieldExAccessibleObjectisAccessible.res - 15;
        }

        return result1;
    }
}

class SampleClassFieldH2 {
    @CustomAnnotationsH2(name = "id")
    String id;
}

@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.FIELD)
@interface CustomAnnotationsH2 {
    String name();
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n