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
 * -@TestCaseID: RTParameterGetAnnotation2
 *- @RequirementName: Java Reflection
 *- @TestCaseName:RTParameterGetAnnotation2.java
 *- @Title/Destination: If expected annotation not exist, getAnnotation() return null; if the given annotation class is
 *                      null, getAnnotation() throws NullPointerException;
 *- @Brief:no:
 * -#step1: 自定义一个类，含有注解的构造方法，和有注解的方法。
 * -#step2：通过调用getDeclaredConstructor获取自定义类的构造方法，通过调用getParameters()获取参数数组，调用
 *          getAnnotation(Class<T> annotationClass)获取注解，参数annotationClass类型与实际注解类型不匹配，确认返回为空。
 * -#step3：通过调用getMethod获取自定义类的方法，通过调用getParameters()获取参数数组，调用
 *          getAnnotation(Class<T> annotationClass)获取注解，参数annotationClass类型与实际注解类型不匹配，确认返回为空。
 * -#step4：通过调用getDeclaredConstructor获取自定义类的构造方法，通过调用getParameters()获取参数数组，调用
 *          getAnnotation(Class<T> annotationClass)并且继续调用注解的方法，参数annotationClass类型与实际注解类型不匹配，
 *          确认抛出NullPointerException异常。
 * -#step5：通过调用getMethod获取自定义类的方法，通过调用getParameters()获取参数数组，调用
 *          getAnnotation(Class<T> annotationClass)获取注解，参数annotationClass为null，确认抛出NullPointerException异常。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: RTParameterGetAnnotation2.java
 *- @ExecuteClass: RTParameterGetAnnotation2
 *- @ExecuteArgs:
 */

import java.lang.annotation.*;
import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.lang.reflect.Parameter;

@Target(ElementType.PARAMETER)
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Inherited
@interface IF2 {
    int i() default 0;
    String t() default "";
}

@Target(ElementType.PARAMETER)
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Inherited
@interface IF2_a {
    int c() default 0;
    String d() default "";
}

class ParameterGetAnnotation2 {
    ParameterGetAnnotation2(@IF2(i = 222, t = "Parameter") int number) {
    }

    public void test1(@IF2_a(c = 666, d = "Happy new year") int age) {
    }
}

public class RTParameterGetAnnotation2 {
    public static void main(String[] args) {
        try {
            Class cls = Class.forName("ParameterGetAnnotation2");
            Constructor cons = cls.getDeclaredConstructor(int.class);
            Method method = cls.getMethod("test1", int.class);
            Parameter[] p1 = cons.getParameters();
            Parameter[] p2 = method.getParameters();
            if (p1[0].getAnnotation(IF2_a.class) == null && p2[0].getAnnotation(IF2.class) == null) {
                p1[0].getAnnotation(IF2_a.class).c();
            }
        } catch (ClassNotFoundException e) {
            System.out.println(1);
            return;
        } catch (NoSuchMethodException e1) {
            System.out.println(2);
            return;
        } catch (NullPointerException e2) {
            try {
                Class cls1 = Class.forName("ParameterGetAnnotation2");
                Method method2 = cls1.getMethod("test1", int.class);
                Parameter[] p3 = method2.getParameters();
                p3[0].getAnnotation(null);
                System.out.println(3);
                return;
            } catch (ClassNotFoundException e3) {
                System.out.println(4);
                return;
            } catch (NoSuchMethodException e4) {
                System.out.println(5);
                return;
            } catch (NullPointerException e5) {
                System.out.println(0);
                return;
            }
        }
        System.out.println(6);
        return;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n