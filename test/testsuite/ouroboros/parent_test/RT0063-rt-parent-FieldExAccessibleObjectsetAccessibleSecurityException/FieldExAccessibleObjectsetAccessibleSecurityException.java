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
 * -@TestCaseID: FieldExAccessibleObjectsetAccessibleSecurityException.java
 * -@TestCaseName: Exception in reflect/Field:      void setAccessible(boolean flag)  \  static void setAccessible(AccessibleObject[] array, boolean flag)
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create a class SampleClassField_h4, which has a field id, and annotate the class with CustomAnnotations_h4
 * -#step2: Call the run of the use case, and execute method 1. In the method, get the filed "id" by calling
 *          getDeclaredField as Object1
 * -#step3: Call setAccessible(boolean flag) on object 1, flag test true and false, confirm set success
 * -#step4: Call the run of the use case, and execute method 1. In the method, get the filed "id" by calling
 *          getDeclaredField as Object2
 * -#step5: Call setAccessible(AccessibleObject[] array, boolean flag) on object 2, flag test true and false, confirm
 *          set success
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: FieldExAccessibleObjectsetAccessibleSecurityException.java
 * -@ExecuteClass: FieldExAccessibleObjectsetAccessibleSecurityException
 * -@ExecuteArgs:
 */


import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;
import java.lang.reflect.AccessibleObject;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;

public class FieldExAccessibleObjectsetAccessibleSecurityException {
    static int res = 99;

    public static void main(String[] args) {
        System.out.println(new FieldExAccessibleObjectsetAccessibleSecurityException().run());
    }

    /**
     * main test fun
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = fieldExAccessibleObjectsetAccessibleSecurityException1();
        } catch (Exception e) {
            FieldExAccessibleObjectsetAccessibleSecurityException.res = FieldExAccessibleObjectsetAccessibleSecurityException.res - 20;
        }

        try {
            result = fieldExAccessibleObjectsetAccessibleSecurityException2();
        } catch (Exception e) {
            FieldExAccessibleObjectsetAccessibleSecurityException.res = FieldExAccessibleObjectsetAccessibleSecurityException.res - 20;
        }

        if (result == 4 && FieldExAccessibleObjectsetAccessibleSecurityException.res == 59) {
            result = 0;
        }

        return result;
    }

    private int fieldExAccessibleObjectsetAccessibleSecurityException1() throws NoSuchFieldException {
        //  void setAccessible(boolean flag)
        int result1 = 4;
        Field f1 = SampleClassFieldH4.class.getDeclaredField("id");
        try {
            f1.setAccessible(false);
            FieldExAccessibleObjectsetAccessibleSecurityException.res = FieldExAccessibleObjectsetAccessibleSecurityException.res - 10;
        } catch (SecurityException e) {
            FieldExAccessibleObjectsetAccessibleSecurityException.res = FieldExAccessibleObjectsetAccessibleSecurityException.res - 15;
        }

        try {
            f1.setAccessible(true);
            FieldExAccessibleObjectsetAccessibleSecurityException.res = FieldExAccessibleObjectsetAccessibleSecurityException.res - 10;
        } catch (SecurityException e) {
            FieldExAccessibleObjectsetAccessibleSecurityException.res = FieldExAccessibleObjectsetAccessibleSecurityException.res - 15;
        }

        return result1;
    }

    private int fieldExAccessibleObjectsetAccessibleSecurityException2() throws NoSuchFieldException {
        //  static void setAccessible(AccessibleObject[] array, boolean flag)
        int result2 = 4;
        Constructor[] test = AccessibleObject.class.getDeclaredConstructors();
        try {
            AccessibleObject.setAccessible(test, false);
            FieldExAccessibleObjectsetAccessibleSecurityException.res = FieldExAccessibleObjectsetAccessibleSecurityException.res - 10;
        } catch (SecurityException e) {
            FieldExAccessibleObjectsetAccessibleSecurityException.res = FieldExAccessibleObjectsetAccessibleSecurityException.res - 15;
        }

        try {
            AccessibleObject.setAccessible(test, true);
            FieldExAccessibleObjectsetAccessibleSecurityException.res = FieldExAccessibleObjectsetAccessibleSecurityException.res - 10;
        } catch (SecurityException e) {
            FieldExAccessibleObjectsetAccessibleSecurityException.res = FieldExAccessibleObjectsetAccessibleSecurityException.res - 15;
        }
        return result2;
    }
}

class SampleClassFieldH4 {
    @CustomAnnotationsH4(name = "id")
    String id;
}

@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.FIELD)
@interface CustomAnnotationsH4 {
    String name();
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n