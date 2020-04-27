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
 * -@TestCaseID: RTMethodGetAnnotation2
 *- @RequirementName: Java Reflection
 *- @TestCaseName:RTMethodGetAnnotation2.java
 *- @Title/Destination: Annotations are not acting at the runtime stage, and the annotation to obtain the target class
 *                      method by reflection is null
 *- @Brief:no:
 * -#step1: 通过Class.forName()方法获取MethodGetAnnotation2类的运行时类clazz；
 * -#step2: 以test3、int.class为参数，通过getDeclaredMethod()方法获取clazz的声明方法并记为method1；
 * -#step3: 以test1为参数，通过getMethod()方法获取clazz的方法对象method2；
 * -#step4: 以IF2_a.class为参数，通过getAnnotation()方法分别获取method1、method2的注解，其返回值均为null；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: RTMethodGetAnnotation2.java
 *- @ExecuteClass: RTMethodGetAnnotation2
 *- @ExecuteArgs:
 */

import java.lang.annotation.*;
import java.lang.reflect.Method;

@Target(ElementType.METHOD)
@Retention(RetentionPolicy.CLASS)
@Documented
@Inherited
@interface IF2 {
    int i() default 0;
    String t() default "";
}

@Target(ElementType.METHOD)
@Retention(RetentionPolicy.SOURCE)
@Documented
@Inherited
@interface IF2_a {
    int c() default 0;
    String d() default "";
}

class MethodGetAnnotation2 {
    @IF2(i = 333, t = "MethodGetAnnotation")
    public void test1() {
    }

    @IF2_a(c = 666, d = "Method")
    void test3(int number) {
    }
}

public class RTMethodGetAnnotation2 {
    public static void main(String[] args) {
        try {
            Class clazz = Class.forName("MethodGetAnnotation2");
            Method method1 = clazz.getDeclaredMethod("test3", int.class);
            Method method2 = clazz.getMethod("test1");
            if (method1.getAnnotation(IF2_a.class) == null && method2.getAnnotation(IF2.class) == null) {
                System.out.println(0);
            }
        } catch (ClassNotFoundException e1) {
            System.err.println(e1);
            System.out.println(2);
        } catch (NoSuchMethodException e2) {
            System.err.println(e2);
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n