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
 * -@TestCaseID: MethodGetDefaultValue
 *- @RequirementName: Java Reflection
 *- @TestCaseName:MethodGetDefaultValue.java
 * - @Title/Destination: Method.getDefaultValue returns the default value for the annotation member represented by this
 *                       Method instance. Returns null if no default is associated with the member。
 * - @Brief:no:
 *  -#step1: 定义含注解的内部类MyTargetTest13。
 *  -#step2：通过调用getMethod()从内部类MyTargetTest13中获取MyTargetTest_1。
 *  -#step3：调用getDefaultValue()获取注解成员的默认值。
 *  -#step4：确认获取为null，无异常抛出。
 * - @Expect: 0\n
 * - @Priority: High
 * - @Source: MethodGetDefaultValue.java
 * - @ExecuteClass: MethodGetDefaultValue
 * - @ExecuteArgs:
 */

import java.lang.annotation.*;
import java.lang.reflect.Method;

public class MethodGetDefaultValue {
    public static void main(String[] argv) {
        int result = 2/* STATUS_FAILED */;
        try {
            result = MethodGetDefaultValueTypeNotPresent_1();
        } catch (Exception e) {
            result = 3;
        }
        System.out.println(result);
    }

    public static int MethodGetDefaultValueTypeNotPresent_1() {
        try {
            Method m = MyTargetTest13.class.getMethod("MyTargetTest_1");
            Object a = m.getDefaultValue();
        } catch (TypeNotPresentException e) {
            return 3;
        } catch (NoSuchMethodException e) {
            return 4;
        }
        return 0;
    }

    @Target(ElementType.TYPE)
    @Retention(RetentionPolicy.RUNTIME)
    public @interface MyTarget {
        Class style() default String.class;
    }

    @MyTarget()
    class MyTargetTest13 {
        public void MyTargetTest_1() {
            System.out.println("This is Example:hello world");
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n