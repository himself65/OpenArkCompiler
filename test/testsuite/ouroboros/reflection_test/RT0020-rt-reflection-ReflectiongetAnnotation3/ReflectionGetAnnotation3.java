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
 * -@TestCaseID: ReflectionGetAnnotation3
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionGetAnnotation3.java
 *- @Title/Destination: Call Class.getAnnotation in functions of target class to get annotation by reflection, if the
 *                      function does not have the expected annotation, throws exception.
 *- @Brief:no:
 *  -#step1: 定义含注解的内部类GetAnnotation3_a。
 *  -#step2：通过调用getField()从内部类GetAnnotation3_a中获取成员。
 *  -#step3：调用getAnnotation(Class<T> annotationClass)获取GetAnnotation3_a的注解并抛出NullPointerException。
 *  -#step4：定义不含注解的类GetAnnotation3_b，通过getAnnotation()方法获取成员方法，并抛出NullPointerException。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionGetAnnotation3.java
 *- @ExecuteClass: ReflectionGetAnnotation3
 *- @ExecuteArgs:
 */

import java.lang.annotation.*;
import java.lang.reflect.Method;

@Target(ElementType.METHOD)
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Inherited
@interface Zzz3 {
    int i() default 0;
    String t() default "";
}

class GetAnnotation3_a {
    @Zzz3(i = 333, t = "getAnnotation")
    public void qqq() {
    };

    public void rrr() {
    };
}

class GetAnnotation3_b {
    public void www() {
    };
}

public class ReflectionGetAnnotation3 {

    public static void main(String[] args) {
        try {
            Class zqp1 = Class.forName("GetAnnotation3_a");
            Method zhu1 = zqp1.getMethod("qqq");
            Method zhu2 = zqp1.getMethod("rrr");
            zhu1.getAnnotation(Zzz3.class).t();
            zhu2.getAnnotation(Zzz3.class).i();
            System.out.println(2);
        } catch (ClassNotFoundException e) {
            System.err.println(e);
            System.out.println(2);
        } catch (NoSuchMethodException e1) {
            System.err.println(e1);
            System.out.println(2);
        } catch (NullPointerException e2) {

            try {
                Class zqp2 = Class.forName("GetAnnotation3_b");
                Method zhu3 = zqp2.getMethod("www");
                zhu3.getAnnotation(Zzz3.class).i();
                System.out.println(2);
            } catch (ClassNotFoundException e3) {
                System.err.println(e3);
                System.out.println(2);
            } catch (NoSuchMethodException e4) {
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