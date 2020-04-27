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
 * -@TestCaseID: MethodGetParameterCount
 *- @RequirementName: Java Reflection
 *- @TestCaseName:MethodGetParameterCount.java
 * - @Title/Destination: Method.getParameterCount() returns the number of formal parameters.
 * - @Brief:no:
 *  -#step1: 定义含注解的内部类MyTargetTest14。
 *  -#step2：通过调用getMethod()从内部类MyTargetTest14中获取MyTargetTest_1。
 *  -#step3：调用getParameterCount()获取参数个数。
 *  -#step4：确认返回的参数个数是0。
 * - @Expect: 0\n
 * - @Priority: High
 * - @Source: MethodGetParameterCount.java
 * - @ExecuteClass: MethodGetParameterCount
 * - @ExecuteArgs:
 */

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.reflect.Method;

public class MethodGetParameterCount {
    @Retention(RetentionPolicy.RUNTIME)
    public @interface MyTarget {
        public String name();
        public String value();
    }

    public static void main(String[] args) {
        int result = 2;

        try {
            result = MethodGetParameterCount1();
        } catch (Exception e) {
            e.printStackTrace();
            result = 3;
        }
        System.out.println(result);
    }

    public static int MethodGetParameterCount1() {
        Method m;
        try {
            m = MyTargetTest14.class.getMethod("MyTargetTest_1");
            int num = m.getParameterCount();
            if (num == 0) {
                return 0;
            }
        } catch (NoSuchMethodException | SecurityException e) {
            e.printStackTrace();
        }
        return 2;
    }

    class MyTargetTest14 {
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
        public MyTargetTest14(String home) {
            this.home = home;
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n