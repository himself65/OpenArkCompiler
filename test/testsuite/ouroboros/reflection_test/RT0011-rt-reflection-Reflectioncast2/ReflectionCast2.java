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
 * -@TestCaseID: ReflectionCast2
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionCast2.java
 *- @Title/Destination: An exception is reported when the parent object cannot be cast to the class represented by this
 *                      class object
 *- @Brief:no:
 * -#step1: 创建一个Cast2类的实例对象cast2,创建一个Cast2类的子类Cast2_a类的实例对象cast2_a；
 * -#step2: 将Cast2_a类的对象cast2_a强制转换为父类Cast2类型；
 * -#step3: step2中的转换不能转换成功，会发生ClassCastException；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionCast2.java
 *- @ExecuteClass: ReflectionCast2
 *- @ExecuteArgs:
 */

class Cast2 {
}

class Cast2_a extends Cast2 {
}

public class ReflectionCast2 {
    public static void main(String[] args) {
        Cast2_a cast2_a = new Cast2_a();
        Cast2 cast2 = new Cast2();
        try {
            cast2_a = Cast2_a.class.cast(cast2);
        } catch (ClassCastException e) {
            System.out.println(0);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n