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
 * -@TestCaseID: ClassGetAnnotationsByType
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ClassGetAnnotationsByType.java
 * - @Title/Destination: Class have no Annotation and method have annotation, Class.getAnnotationsByType() return an
 *                       array of length 0.
 * - @Brief:no:
 *  -#step1: 定义含注解的内部类MyTargetTest0。
 *  -#step2：获取class MyTargetTest0。
 *  -#step3：调用getAnnotationsByType(Class<T> annotationClass)获取类型为MyTarget的注解数组。
 *  -#step4：确认获取的注解数组个数大于0。
 * - @Expect: 0\n
 * - @Priority: High
 * - @Source: ClassGetAnnotationsByType.java
 * - @ExecuteClass: ClassGetAnnotationsByType
 * - @ExecuteArgs:
 */

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

public class ClassGetAnnotationsByType {
    @Retention(RetentionPolicy.RUNTIME)
    public @interface MyTarget {
        public String name();
        public String value();
    }

    public static void main(String[] args) {
        int result = 2;
        try {
            result = ClassGetAnnotationsByType1();
        } catch (Exception e) {
            e.printStackTrace();
            result = 3;
        }
        System.out.println(result);
    }

    public static int ClassGetAnnotationsByType1() {
        Class<MyTargetTest0> m;
        try {
            m = MyTargetTest0.class;
            MyTarget[] Target = m.getAnnotationsByType(MyTarget.class);
            if (Target.length == 0) {
                return 0;
            }
        } catch (SecurityException e) {
            e.printStackTrace();
        }
        return 2;
    }

    class MyTargetTest0 {
        @MyTarget(name = "newName", value = "newValue")
        public String home;

        @MyTarget(name = "name", value = "value")
        public void MyTargetTest_1() {
            System.out.println("This is Example:hello world");
        }

        public void newMethod(@MyTarget(name = "name1", value = "value1") String home) {
            System.out.println("my home at:" + home);
        }

        @MyTarget(name = "cons", value = "constructor")
        public MyTargetTest0(String home) {
            this.home = home;
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n