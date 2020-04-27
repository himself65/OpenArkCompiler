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
 * -@TestCaseID: ReflectionGetDeclaredAnnotations1
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionGetDeclaredAnnotations1.java
 *- @Title/Destination: Class.GetDeclaredAnnotations() retrieve all annotations from the target class by reflection,
 *                      and result an array of annotations.
 *- @Brief:no:
 * -#step1: Define two annotations.
 * -#step2: Use classloader to load class.R
 * -#step3: Return an array of annotations by calling GetDeclaredAnnotations().
 * -#step4: Check that Class.GetDeclaredAnnotations() retrieves all annotations.
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionGetDeclaredAnnotations1.java
 *- @ExecuteClass: ReflectionGetDeclaredAnnotations1
 *- @ExecuteArgs:
 */

import java.lang.annotation.*;

@Target(ElementType.TYPE)
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Inherited
@interface Ddd1 {
    int i() default 0;

    String t() default "";
}

@Target(ElementType.TYPE)
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Inherited
@interface Ddd1_a {
    int i_a() default 2;

    String t_a() default "";
}

@Ddd1(i = 333, t = "test1")
class GetDeclaredAnnotations1 {
    public int i;
    public String t;
}

@Ddd1_a(i_a = 666, t_a = "right1")
class GetDeclaredAnnotations1_a extends GetDeclaredAnnotations1 {
    public int i_a;
    public String t_a;
}

public class ReflectionGetDeclaredAnnotations1 {

    public static void main(String[] args) {
        int result = 0; /* STATUS_Success */
        try {
            Class zqp1 = Class.forName("GetDeclaredAnnotations1_a");
            if (zqp1.getDeclaredAnnotations().length == 1) {
                Annotation[] j = zqp1.getDeclaredAnnotations();
                if (j[0].toString().indexOf("i_a=666") != -1 && j[0].toString().indexOf("t_a=right1") != -1) {
                    result = 0;
                }
            }
        } catch (ClassNotFoundException e) {
            System.err.println(e);
            result = -1;
        } catch (NullPointerException e2) {
            System.err.println(e2);
            result = -1;
        }
        System.out.println(result);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n