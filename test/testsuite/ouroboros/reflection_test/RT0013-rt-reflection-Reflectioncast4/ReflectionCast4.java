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
 * -@TestCaseID: ReflectionCast4
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionCast4.java
 *- @Title/Destination: Object cast to NULL
 *- @Brief:no:
 * -#step1: 创建一个Cast4类的实例对象cast4，创建一个Object类的变量object，并赋初值为null；
 * -#step2: 将Cast4类的实例对象cast4强制转换为null；
 * -#step3: step2中成功将cast4对象转换为null；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionCast4.java
 *- @ExecuteClass: ReflectionCast4
 *- @ExecuteArgs:
 */

class Cast4 {
}

public class ReflectionCast4 {
    public static void main(String[] args) {
        Object object = null;
        Cast4 cast4 = new Cast4();
        cast4 = Cast4.class.cast(object);
        if (cast4 == null) {
            System.out.println(0);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n