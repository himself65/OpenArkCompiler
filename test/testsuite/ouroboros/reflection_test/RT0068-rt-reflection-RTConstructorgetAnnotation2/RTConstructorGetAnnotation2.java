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
 * -@TestCaseID: RTConstructorGetAnnotation2
 *- @RequirementName: Java Reflection
 *- @TestCaseName:RTConstructorGetAnnotation2.java
 *- @Title/Destination: When try to get annotation of target class's constructor by reflection and the annotation does
 *                      not exist, Constructor.getAnnotation() throws NullPointerException.
 *- @Brief:no
 * -#step1: 定义类1有含注解的构造方法，类2有无注解的构造方法。
 * -#step2：通过Class.forName获取类1的class。
 * -#step3：通过getDeclaredConstructor(Class...<?> parameterTypes)获取构造方法1，通过getConstructor()获取无参构造方法2、
 * -#step4：构造方法1，2调用getAnnotation(Class<T> annotationClass)，annotationClass为不匹配的注解类型，判断返回的注解为空。
 * -#step5：通过Class.forName获取类2的class。
 * -#step6：通过getDeclaredConstructor(Class...<?> parameterTypes)获取构造方法3。
 * -#step7：构造方法3调用getAnnotation(Class<T> annotationClass)，annotationClass为注解类型，判断返回的注解为空。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: RTConstructorGetAnnotation2.java
 *- @ExecuteClass: RTConstructorGetAnnotation2
 *- @ExecuteArgs:
 */

import java.lang.annotation.*;
import java.lang.reflect.Constructor;

@Target(ElementType.CONSTRUCTOR)
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Inherited
@interface IF2 {
    int i() default 0;
    String t() default "";
}

@Target(ElementType.CONSTRUCTOR)
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Inherited
@interface IF2_a {
    int c() default 0;
    String d() default "";
}

class ConstructorGetAnnotation2 {
    @IF2(i = 333, t = "ConstructorGetAnnotation")
    public ConstructorGetAnnotation2() {
    }

    @IF2_a(c = 666, d = "Constructor")
    ConstructorGetAnnotation2(int number) {
    }
}

class ConstructorGetAnnotation2_a {
    ConstructorGetAnnotation2_a(String name, int number) {
    }
}

public class RTConstructorGetAnnotation2 {
    public static void main(String[] args) {
        try {
            Class cls = Class.forName("ConstructorGetAnnotation2");
            Constructor instance1 = cls.getDeclaredConstructor(int.class);
            Constructor instance2 = cls.getConstructor();
            if (instance1.getAnnotation(IF2.class) == null && instance2.getAnnotation(IF2_a.class) == null) {
                instance1.getAnnotation(IF2.class).toString();
            }
            System.out.println(1);
            return;
        } catch (ClassNotFoundException e1) {
            System.out.println(1);
            return;
        } catch (NoSuchMethodException e2) {
            System.out.println(1);
            return;
        } catch (NullPointerException e3) {
            try {
                Class cls = Class.forName("ConstructorGetAnnotation2_a");
                Constructor instance3 = cls.getDeclaredConstructor(String.class, int.class);
                if (instance3.getAnnotation(IF2.class) == null && instance3.getAnnotation(IF2_a.class) == null) {
                    instance3.getAnnotation(IF2.class).toString();
                }
                System.out.println(1);
                return;
            } catch (ClassNotFoundException e4) {
                System.out.println(2);
                return;
            } catch (NoSuchMethodException e5) {
                System.out.println(3);
                return;
            } catch (NullPointerException e6) {
                System.out.println(0);
                return;
            }
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n