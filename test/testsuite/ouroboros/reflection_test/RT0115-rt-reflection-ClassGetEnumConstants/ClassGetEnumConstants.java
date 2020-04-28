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
 * -@TestCaseID: ClassGetEnumConstants
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ClassGetEnumConstants.java
 * - @Title/Destination: Class getEnumConstants() return null when there is no enum in the class。
 * - @Brief:no:
 *  -#step1: 定义含注解的内部类MyTargetTest3。
 *  -#step2：获取class MyTargetTest3。
 *  -#step3：调用getEnumConstants()获取枚举类的元素。
 *  -#step4：确认返回为空。
 * - @Expect: 0\n
 * - @Priority: High
 * - @Source: ClassGetEnumConstants.java
 * - @ExecuteClass: ClassGetEnumConstants
 * - @ExecuteArgs:
 */

import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;

public class ClassGetEnumConstants {
    @Retention(RetentionPolicy.RUNTIME)
    public @interface MyTarget {
        public String name();
        public String value();
    }

    public static void main(String[] args) {
        int result = 2;
        try {
            result = ClassGetEnumConstants1();
        } catch (Exception e) {
            e.printStackTrace();
            result = 3;
        }
        System.out.println(result);
    }

    public static int ClassGetEnumConstants1() {
        Class<MyTargetTest3> m;
        try {
            m = MyTargetTest3.class;
            MyTargetTest3[] target = m.getEnumConstants();
            if (target == null) {
                return 0;
            }
        } catch (SecurityException e) {
            e.printStackTrace();
        }
        return 2;
    }

    class MyTargetTest3 {
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
        public MyTargetTest3(String home) {
            this.home = home;
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n