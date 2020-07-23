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
 * -@TestCaseID: ReflectionGetSigners
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionGetSigners.java
 *- @Title/Destination: Gets the signers of this class.
 *- @Brief:no:
 * -#step1: 通过Class.forName()方法获取GetSigners类的运行时类clazz；
 * -#step2: 通过getSigners()方法获取clazz的所有的标签并记为objects；
 * -#step3: 确定step2中成功获取到objects，并且objects等于null；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionGetSigners.java
 *- @ExecuteClass: ReflectionGetSigners
 *- @ExecuteArgs:
 */

class GetSigners {
}

public class ReflectionGetSigners {
    public static void main(String[] args) {
        try {
            Class clazz = Class.forName("GetSigners");
            Object[] objects = clazz.getSigners();
            if (objects == null) {
                System.out.println(0);
            }
        } catch (ClassNotFoundException e) {
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n