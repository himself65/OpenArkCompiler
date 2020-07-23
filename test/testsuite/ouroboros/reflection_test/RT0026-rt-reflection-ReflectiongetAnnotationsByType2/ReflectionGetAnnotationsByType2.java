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
 * -@TestCaseID: ReflectionGetAnnotationsByType2
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionGetAnnotationsByType2.java
 *- @Title/Destination: Retrieving an empty annotation of a field in a target class by reflection reports an exception.
 *                      Class.GetAnnotationsByType(null) throws NullPointerException.
 *- @Brief:no:
 *  -#step1: 创建含注解的类GetAnnotationsByType2_a，不含注解的类GetAnnotationsByType2_b。
 *  -#step2: 通过forName()获取GetAnnotationsByType2_a的类名对象，通过getField()获取GetAnnotationsByType2_a内对应名称的成员。
 *  -#step3: 调用getAnnotationsByType(Class<T> annotationClass)获取类型为MyTarget的数组并抛出NullPointerException。
 *  -#step4：通过forName()获取GetAnnotationsByType2_b的类名对象，通过getField()获取GetAnnotationsByType2_b内对应名称的成员。
 *  -#step5：调用getAnnotationsByType(Class<T> annotationClass)获取类型为MyTarget的数组并抛出NullPointerException。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionGetAnnotationsByType2.java
 *- @ExecuteClass: ReflectionGetAnnotationsByType2
 *- @ExecuteArgs:
 */

import java.lang.annotation.*;
import java.lang.reflect.Field;

@Target(ElementType.FIELD)
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Inherited
@interface Zzzz2 {
    int i() default 0;

    String t() default "";
}

class GetAnnotationsByType2_a {
    @Zzzz2(i = 333, t = "GetAnnotationsByType")
    public int i_a;
    public String t_a;
}

class GetAnnotationsByType2_b {
    public int i_b;
    public String t_b;
}

public class ReflectionGetAnnotationsByType2 {

    public static void main(String[] args) {
        try {
            Class zqp1 = Class.forName("GetAnnotationsByType2_a");
            Field zhu1 = zqp1.getField("t_a");
            Annotation[] j = zhu1.getAnnotationsByType(null);
            System.out.println(2);
        } catch (ClassNotFoundException e) {
            System.err.println(e);
            System.out.println(2);
        } catch (NoSuchFieldException e1) {
            System.err.println(e1);
            System.out.println(2);
        } catch (NullPointerException e2) {

            try {
                Class zqp2 = Class.forName("GetAnnotationsByType2_b");
                Field zhu2 = zqp2.getField("i_b");
                Annotation[] k = zhu2.getAnnotationsByType(null);
                System.out.println(2);
            } catch (ClassNotFoundException e3) {
                System.err.println(e3);
                System.out.println(2);
            } catch (NoSuchFieldException e4) {
                System.err.println(e4);
                System.out.println(2);
            } catch (NullPointerException e5) {
                System.out.println(0);
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n