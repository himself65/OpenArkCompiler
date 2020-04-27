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
 * -@TestCaseID: RTConstructorGetDeclaredAnnotations2
 *- @RequirementName: Java Reflection
 *- @TestCaseName:RTConstructorGetDeclaredAnnotations2.java
 *- @Title/Destination: Constructor.getDeclaredAnnotations() does not return annotations inherited from parent class.
 *- @Brief:no:
 * -#step1: 定义两个类，有父子关系，父类含有注解的构造方法，子类的构造方法没有注解。
 * -#step2：调用getConstructor(Class...<?> parameterTypes)从子类中获取构造方法1。
 * -#step3：调用getDeclaredAnnotations()获取注解数组，判断数组长度为0。
 * -#step4：调用getConstructor(Class...<?> parameterTypes)从子类中获取构造方法2。
 * -#step5：调用getDeclaredAnnotations()获取注解数组，判断数组长度为0。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: RTConstructorGetDeclaredAnnotations2.java
 *- @ExecuteClass: RTConstructorGetDeclaredAnnotations2
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
    int i_a() default 2;
    String t_a() default "";
}

class ConstructorGetDeclaredAnnotations2 {
    public ConstructorGetDeclaredAnnotations2() {
    }

    @IF2(i = 333, t = "test1")
    public ConstructorGetDeclaredAnnotations2(String name) {
    }

    @IF2(i = 333, t = "test1")
    ConstructorGetDeclaredAnnotations2(int number) {
    }
}

class ConstructorGetDeclaredAnnotations2_a extends ConstructorGetDeclaredAnnotations2 {
    public ConstructorGetDeclaredAnnotations2_a(String name) {
    }

    ConstructorGetDeclaredAnnotations2_a(int number) {
    }
}

public class RTConstructorGetDeclaredAnnotations2 {
    public static void main(String[] args) {
        try {
            Class cls = Class.forName("ConstructorGetDeclaredAnnotations2_a");
            Constructor instance1 = cls.getConstructor(String.class);
            Constructor instance2 = cls.getDeclaredConstructor(int.class);
            if (instance1.getDeclaredAnnotations().length == 0 && instance2.getDeclaredAnnotations().length == 0) {
                System.out.println(0);
                return;
            }
            System.out.println(1);
            return;
        } catch (ClassNotFoundException e) {
            System.out.println(2);
            return;
        } catch (NoSuchMethodException e1) {
            System.out.println(3);
            return;
        } catch (NullPointerException e2) {
            System.out.println(4);
            return;
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n