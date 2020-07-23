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
 * -@TestCaseID: ClassInitMethodInvokeStatic
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ClassInitMethodInvokeStatic.java
 *- @Title/Destination: When m is a static method of class One and call m.invoke(null, args), class One is initialized.
 *- @Brief:no:
 * -#step1: Class.forName("One" , false, One.class.getClassLoader()) and clazz.getDeclaredMethod to get a static method
 *          m of class One.
 * -#step2: Call methods of Method except invoke(), class One is not initialized.
 * -#step3: Call method invoke(), class One is initialized.
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ClassInitMethodInvokeStatic.java
 *- @ExecuteClass: ClassInitMethodInvokeStatic
 *- @ExecuteArgs:
 */

import java.lang.annotation.*;
import java.lang.reflect.Method;

public class ClassInitMethodInvokeStatic {
    static StringBuffer result = new StringBuffer("");

    public static void main(String[] args) {
        try {
            Class clazz = Class.forName("One", false, One.class.getClassLoader());
            Method m = clazz.getDeclaredMethod("testOne", String.class);
            // Check point 1: calling following methods, class not initialized.
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

            // Check point 2: after newInstance, class initialized.
            if (result.toString().compareTo("") == 0) {
                m.invoke(null, "hi");
            }
        } catch (Exception e) {
            System.out.println(e);
        }

        if (result.toString().compareTo("SuperOne") == 0) {
            System.out.println(0);
        } else {
            System.out.println(2);
        }
    }
}

@A
class Super {
    static {
        ClassInitMethodInvokeStatic.result.append("Super");
    }
}

interface InterfaceSuper {
    String a = ClassInitMethodInvokeStatic.result.append("|InterfaceSuper|").toString();
}

@A(i = 1)
class One extends Super implements InterfaceSuper {
    static {
        ClassInitMethodInvokeStatic.result.append("One");
    }

    public static int testOne(String a) {
        return 0;
    }
}

@Target(ElementType.TYPE)
@Retention(RetentionPolicy.RUNTIME)
@interface A {
    int i() default 0;
    String a = ClassInitMethodInvokeStatic.result.append("|InterfaceA|").toString();
}

class Two extends One {
    static {
        ClassInitMethodInvokeStatic.result.append("Two");
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n