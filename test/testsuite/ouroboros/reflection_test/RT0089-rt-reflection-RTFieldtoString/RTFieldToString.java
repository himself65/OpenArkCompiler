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
 * -@TestCaseID: RTFieldToString
 *- @RequirementName: Java Reflection
 *- @TestCaseName:RTFieldToString.java
 *- @Title/Destination: Field.ToString() returns a string describing this Field.
 *- @Brief:no:
 * -#step1: 定义含私有变量的类FieldToGenericString<E, F>。
 * -#step2: 通过调用forName()方法加载类FieldToGenericString，调用getField()、getDeclaredField()分别获取相应的成员。
 * -#step3: 通过调用toString()()返回对象的string，确认字符串描述正确。
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: RTFieldToString.java
 *- @ExecuteClass: RTFieldToString
 *- @ExecuteArgs:
 */

import java.lang.reflect.Field;
import java.util.List;

class FieldToString<E, F> {
    public List<F> list1;
    private List<E> list2;
}

public class RTFieldToString {
    public static void main(String[] args) {
        try {
            Class cls = Class.forName("FieldToString");
            Field instance1 = cls.getField("list1");
            Field instance2 = cls.getDeclaredField("list2");
            String q1 = instance1.toString();
            String q2 = instance2.toString();
            if (q1.equals("public java.util.List FieldToString.list1")
                    && q2.equals("private java.util.List FieldToString.list2")) {
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
// ASSERT: scan 0\n