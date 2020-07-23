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
 * -@TestCaseID: RTFieldGetType
 *- @RequirementName: Java Reflection
 *- @TestCaseName:RTFieldGetType.java
 *- @Title/Destination: Field.GetType() Returns a Class object that identifies the declared type for the field
 *                      represented by this Field object.
 *- @Brief:no:
 * -#step1: 定义类FieldGetType。
 * -#step2: 调用forName()方法加载类FieldGetType。
 * -#step3: 调用getField()、getDeclaredField()方法获取对应的成员变量。
 * -#step4: 调用getType().getName()获取相应的类型,调用equals()方法与相应的类型比较，判断通过字段值正确。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: RTFieldGetType.java
 *- @ExecuteClass: RTFieldGetType
 *- @ExecuteArgs:
 */

import java.lang.reflect.Field;

class FieldGetType {
    public static short sNum;
    final char aChar = '什';
    private float fNum;
}

public class RTFieldGetType {
    public static void main(String[] args) {
        try {
            Class cls = Class.forName("FieldGetType");
            Field instance1 = cls.getField("sNum");
            Field instance2 = cls.getDeclaredField("aChar");
            Field instance3 = cls.getDeclaredField("fNum");
            if (instance1.getType().getName().equals("short") && instance2.getType().getName().equals("char")
                    && instance3.getType().getName().equals("float")) {
                System.out.println(0);
            }
        } catch (ClassNotFoundException e1) {
            System.err.println(e1);
            System.out.println(2);
        } catch (NoSuchFieldException e2) {
            System.err.println(e2);
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n