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
 * -@TestCaseID: RTParameterGetDeclaredAnnotations2
 *- @RequirementName: Java Reflection
 *- @TestCaseName:RTParameterGetDeclaredAnnotations2.java
 *- @Title/Destination: Parameter.getDeclaredAnnotations() returns annotations that are directly present on this element.
 *                      This method ignores inherited annotations. If there are no annotations directly present on this
 *                      element, the return value is an array of length 0.
 *- @Brief:no:
 * -#step1：创建一个类1，含有注解和无注解的构造方法和有注解方法。创建一个子类2继承类1，含有注解和无注解的构造方法和有注
 *          解方法。创建子类3继承类2，重写构造方法和方法，均为无注解。
 * -#step2：通过调用getDeclaredConstructor获取自定义类3的构造方法，通过调用getParameters()获取参数数组，调用
 *          getDeclaredAnnotations()获取注解数组，检查数组长度为0。
 * -#step3：通过调用getMethod获取自定义类的方法，通过调用getParameters()获取参数数组，调用getDeclaredAnnotations()获取注
 *          解数组，检查数组长度为0。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: RTParameterGetDeclaredAnnotations2.java
 *- @ExecuteClass: RTParameterGetDeclaredAnnotations2
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
@interface IF4 {
    int i() default 0;
    String t() default "";
}

@Target(ElementType.PARAMETER)
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Inherited
@interface IF4_a {
    int c() default 0;
    String d() default "";
}

class ParameterGetDeclaredAnnotations2 {
    ParameterGetDeclaredAnnotations2() {
    }

    ParameterGetDeclaredAnnotations2(@IF4(i = 222, t = "Parameter") int number) {
    }

    public void test1(@IF4(i = 222, t = "Parameter") String name) {
    }
}

class ParameterGetDeclaredAnnotations2_a extends ParameterGetDeclaredAnnotations2 {
    ParameterGetDeclaredAnnotations2_a() {
    }

    ParameterGetDeclaredAnnotations2_a(@IF4_a(c = 666, d = "Happy new year") int number) {
    }

    public void test1(@IF4_a(c = 666, d = "Happy new year") String name) {
    }
}

class ParameterGetDeclaredAnnotations2_b extends ParameterGetDeclaredAnnotations2_a {
    ParameterGetDeclaredAnnotations2_b(int number) {
    }

    public void test1(String name) {
    }
}

public class RTParameterGetDeclaredAnnotations2 {
    public static void main(String[] args) {
        try {
            Class cls = Class.forName("ParameterGetDeclaredAnnotations2_b");
            Constructor cons = cls.getDeclaredConstructor(int.class);
            Method method = cls.getMethod("test1", String.class);
            Parameter[] p1 = cons.getParameters();
            Parameter[] p2 = method.getParameters();
            if (p1[0].getDeclaredAnnotations().length == 0 && p2[0].getDeclaredAnnotations().length == 0) {
                System.out.println(0);
                return;
            }
        } catch (ClassNotFoundException e) {
            System.out.println(1);
            return;
        } catch (NoSuchMethodException e1) {
            System.out.println(2);
            return;
        }
        System.out.println(3);
        return;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n