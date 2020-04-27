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
 * -@TestCaseID: ClassInitMethodInvokeInterface
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ClassInitMethodInvokeInterface.java
 *- @Title/Destination: When m is a static method of interface OneInterface and call m.invoke(), OneInterface is
 *                      initialized, it's parent interface is not initialized.
 *- @Brief:no:
 * -#step1: Class.forName("OneInterface", false, OneInterface.class.getClassLoader()) and clazz.getDeclaredMethod to get
 *          a static method m of OneInterface.
 * -#step2: Call methods of Method except invoke(), class One is not initialized.
 * -#step3: Call method invoke(), class One is initialized
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ClassInitMethodInvokeInterface.java
 *- @ExecuteClass: ClassInitMethodInvokeInterface
 *- @ExecuteArgs:
 */

import java.lang.reflect.Method;

public class ClassInitMethodInvokeInterface {
    static StringBuffer result = new StringBuffer("");

    public static void main(String[] args) {
        try {
            Class clazz = Class.forName("OneInterface", false, OneInterface.class.getClassLoader());
            Method m = clazz.getDeclaredMethod("runInterface", String.class);
            //check point 1: calling following methods, class not initialized
            m.equals(m);
            m.getAnnotation(A.class);
            m.getDeclaredAnnotations();
            m.getDeclaringClass();
            m.getDefaultValue();
            m.getExceptionTypes();
            m.getGenericExceptionTypes();
            m.getGenericParameterTypes();
            m.getGenericReturnType();
            m.getModifiers();
            m.getName();
            m.getParameterAnnotations();
            m.getParameterCount();
            m.getParameterTypes();
            m.getReturnType();
            m.getTypeParameters();
            m.hashCode();
            m.isBridge();
            m.isDefault();
            m.isSynthetic();
            m.isVarArgs();
            m.toString();
            m.toGenericString();

            //check point 2: after newInstance, class initialized
            if (result.toString().compareTo("") == 0) {
                m.invoke(null, "hi");
            }
        } catch (Exception e) {
            System.out.println(e);
        }

        if (result.toString().compareTo("OneRunInterface") == 0) {
            System.out.println(0);
        } else {
            System.out.println(2);
        }
    }
}

interface SuperInterface {
    String aSuper = ClassInitMethodInvokeInterface.result.append("Super").toString();
}

interface OneInterface extends SuperInterface {
    String aOne = ClassInitMethodInvokeInterface.result.append("One").toString();
    @A
    static void runInterface(String a) {
        ClassInitMethodInvokeInterface.result.append("RunInterface");
    }
}

interface TwoInterface extends OneInterface {
    String aTwo = ClassInitMethodInvokeInterface.result.append("Two").toString();
}

@interface A {
    String aA = ClassInitMethodInvokeInterface.result.append("Annotation").toString();
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n