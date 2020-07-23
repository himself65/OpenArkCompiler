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
 * -@TestCaseID: ClassInitFieldOtherMethodInterface
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ClassInitFieldOtherMethodInterface.java
 *- @Title/Destination: When f is a field of of interface OneInterface and call method except setXX/getXX, class is not
 *                      initialized.
 *- @Brief:no:
 * -#step1: Class.forName("OneInterface", false, OneInterface.class.getClassLoader()) and clazz.getField to get field f
 *          of OneInterface.
 * -#step2: Call method of Field except setXX/getXX, class One is not initialized.
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ClassInitFieldOtherMethodInterface.java
 *- @ExecuteClass: ClassInitFieldOtherMethodInterface
 *- @ExecuteArgs:
 */

import java.lang.reflect.Field;

public class ClassInitFieldOtherMethodInterface {
    static StringBuffer result = new StringBuffer("");

    public static void main(String[] args) {
        try {
            Class clazz = Class.forName("OneInterface", false, OneInterface.class.getClassLoader());
            Field f = clazz.getField("hi");

            f.equals(f);
            f.getAnnotation(A.class);
            f.getAnnotationsByType(A.class);
            f.getDeclaredAnnotations();
            f.getDeclaringClass();
            f.getGenericType();
            f.getModifiers();
            f.getName();
            f.getType();
            f.hashCode();
            f.isEnumConstant();
            f.isSynthetic();
            f.toGenericString();
            f.toString();
        } catch (Exception e) {
            System.out.println(e);
        }

        if (result.toString().compareTo("") == 0) {
            System.out.println(0);
        } else {
            System.out.println(2);
        }
    }
}

interface SuperInterface {
    String aSuper = ClassInitFieldOtherMethodInterface.result.append("Super").toString();
}

interface OneInterface extends SuperInterface {
    String aOne = ClassInitFieldOtherMethodInterface.result.append("One").toString();
    @A
    short hi = 14;
}

interface TwoInterface extends OneInterface {
    String aTwo = ClassInitFieldOtherMethodInterface.result.append("Two").toString();
}

@interface A {
    String aA = ClassInitFieldOtherMethodInterface.result.append("Annotation").toString();
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n