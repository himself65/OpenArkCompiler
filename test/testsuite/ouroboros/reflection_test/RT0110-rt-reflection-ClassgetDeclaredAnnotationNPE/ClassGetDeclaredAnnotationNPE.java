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
 * -@TestCaseID: ClassGetDeclaredAnnotationNPE
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ClassGetDeclaredAnnotationNPE.java
 *- @Title/Destination: Class.getDeclaredAnnotation(null) throws NullPointerException
 *- @Brief:no:
 *  -#step1: 定义内部类MyClass, 获取class MyClass。
 *  -#step2：调用getDeclaredAnnotation(Class<A> annotationClass), annotationClass为Deprecated.class。
 *  -#step3：确认无异常抛出。
 *  -#step4：调用getDeclaredAnnotation(Class<A> annotationClass), annotationClass为null。
 *  -#step5：确认抛出NullPointerException。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ClassGetDeclaredAnnotationNPE.java
 *- @ExecuteClass: ClassGetDeclaredAnnotationNPE
 *- @ExecuteArgs:
 */

import java.lang.annotation.Annotation;

public class ClassGetDeclaredAnnotationNPE {
    public static void main(String argv[]) {
        int result = 2; /* STATUS_FAILED */
        try {
            result = ClassGetDeclaredAnnotationNPE_1();
        } catch (Exception e) {
            result = 3;
        }
        System.out.println(result);
    }

    public static int ClassGetDeclaredAnnotationNPE_1() {
        try {
            Annotation a = MyClass.class.getDeclaredAnnotation(Deprecated.class);
            a = MyClass.class.getDeclaredAnnotation(null);
        } catch (NullPointerException e) {
            return 0;
        }
        return 4;
    }

    @Deprecated
    class MyClass extends MySuperClass {
    }

    @MyAnnotation
    class MySuperClass {
    }

    @interface MyAnnotation {
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n