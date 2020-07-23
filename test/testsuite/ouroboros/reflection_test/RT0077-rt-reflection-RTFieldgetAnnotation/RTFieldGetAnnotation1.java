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
 * -@TestCaseID: RTFieldGetAnnotation1
 *- @RequirementName: Java Reflection
 *- @TestCaseName:RTFieldGetAnnotation1.java
 *- @Title/Destination: Field.GetAnnotation() returns null when the expected annotation is not present.
 *- @Brief:no:
 * -#step1: 定义含注解类FieldGetAnnotation1。
 * -#step2: 通过调用forName()方法加载类FieldGetAnnotation1。
 * -#step3: 通过调用getField()获取对应名称的成员变量。
 * -#step4: 调用getAnnotation(Class<T> annotationClass)，annotationClass为不匹配的类型，确认返回null。
 * -#step5: 调用返回空注解类型进行.number()的调用，确认抛出NullPointerException。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: RTFieldGetAnnotation1.java
 *- @ExecuteClass: RTFieldGetAnnotation1
 *- @ExecuteArgs:
 */

import java.lang.annotation.*;
import java.lang.reflect.Field;

@Target(ElementType.FIELD)
@Retention(RetentionPolicy.RUNTIME)
@Documented
@Inherited
@interface interface4 {
    int num() default 0;
    String str() default "";
}

@interface interface4_a {
    int number() default 0;
    String string() default "";
}

class FieldGetAnnotation1 {
    @interface4(num = 333, str = "GetAnnotation")
    public int num1;
    @interface4_a(number = 555, string = "test")
    public String str1;
}

public class RTFieldGetAnnotation1 {
    public static void main(String[] args) {
        try {
            Class cls1 = Class.forName("FieldGetAnnotation1");
            Field instance1 = cls1.getField("num1");
            Field instance2 = cls1.getField("str1");
            if (instance1.getAnnotation(interface4_a.class) == null && instance2.getAnnotation(interface4.class) == null) {
                instance1.getAnnotation(interface4_a.class).number();
            }
            System.out.println(2);
        } catch (ClassNotFoundException e) {
            System.err.println(e);
            System.out.println(2);
        } catch (NoSuchFieldException e1) {
            System.err.println(e1);
            System.out.println(2);
        } catch (NullPointerException e2) {
            System.out.println(0);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n