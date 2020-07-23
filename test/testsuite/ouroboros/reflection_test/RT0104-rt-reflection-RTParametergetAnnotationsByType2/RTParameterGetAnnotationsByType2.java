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
 * -@TestCaseID: RTParameterGetAnnotationsByType2
 *- @RequirementName: Java Reflection
 *- @TestCaseName:RTParameterGetAnnotationsByType2.java
 *- @Title/Destination:Parameter.getAnnotationsByType() return an array of length zero if no expected annotation found;
 *                     getAnnotationsByType(null) throws NullPointerException.
 *- @Brief:no:
 * -#step1: 自定义一个类，含有注解的构造方法，和不含注解的方法。
 * -#step2：通过调用getDeclaredConstructor获取自定义类的构造方法，通过调用getParameters()获取参数数组，调用
 *          getAnnotationsByType(Class<T> annotationClass)获取注解数组，annotationClass为非对应的注解类型，对注解数组q1
 *          调用q1[0].toString(), 检查会有ArrayIndexOutOfBoundsException抛出。
 * -#step3：通过调用getMethod获取自定义类的方法，通过调用getParameters()获取参数数组，调用
 *          getAnnotationsByType(Class<T> annotationClass)获取注解数组，annotationClass为非对应的注解类型，对注解数组q2
 *          调用q2[0].toString(), 检查会有ArrayIndexOutOfBoundsException抛出。
 * -#step4：通过调用getDeclaredConstructor获取自定义类的构造方法，通过调用getParameters()获取参数数组，调用
 *          getAnnotationsByType(Class<T> annotationClass)获取注解数组，annotationClass为null，检查会有
 *          NullPointerException抛出。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: RTParameterGetAnnotationsByType2.java
 *- @ExecuteClass: RTParameterGetAnnotationsByType2
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
@interface IF8 {
    int i() default 0;
    String t() default "";
}

@Target(ElementType.PARAMETER)
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Inherited
@interface IF8_a {
    int c() default 0;
    String d() default "";
}

class ParameterGetAnnotationsByType2 {
    ParameterGetAnnotationsByType2(@IF8(i = 222, t = "Parameter") int number) {
    }

    public void test1(@IF8_a(c = 666, d = "Happy new year") int age) {
    }
}

public class RTParameterGetAnnotationsByType2 {
    public static void main(String[] args) {
        try {
            Class cls = Class.forName("ParameterGetAnnotationsByType2");
            Constructor cons = cls.getDeclaredConstructor(int.class);
            Method method = cls.getMethod("test1", int.class);
            Parameter[] p1 = cons.getParameters();
            Parameter[] p2 = method.getParameters();
            Annotation[] q1 = p1[0].getAnnotationsByType(IF8_a.class);
            Annotation[] q2 = p2[0].getAnnotationsByType(IF8.class);
            q1[0].toString();
            q2[0].toString();
        } catch (ClassNotFoundException e) {
            System.out.println(1);
            return;
        } catch (NoSuchMethodException e1) {
            System.out.println(2);
            return;
        } catch (ArrayIndexOutOfBoundsException e2) {
            try {
                Class cls1 = Class.forName("ParameterGetAnnotationsByType2");
                Constructor cons1 = cls1.getDeclaredConstructor(int.class);
                Parameter[] p3 = cons1.getParameters();
                Annotation[] q3 = p3[0].getAnnotationsByType(null);
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