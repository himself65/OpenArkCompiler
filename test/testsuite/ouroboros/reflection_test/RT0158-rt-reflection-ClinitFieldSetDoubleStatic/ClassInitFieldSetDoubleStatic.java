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
 * -@TestCaseID: ClassInitFieldSetDoubleStatic
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ClassInitFieldSetDoubleStatic.java
 *- @Title/Destination: When f is a static field of class One and call f.setDouble(), class One is initialized.
 *- @Brief:no:
 * -#step1: Class.forName("One" , false, One.class.getClassLoader()) and clazz.getField to get a static field f of class
 *          One.
 * -#step2: Call method f.setDouble(null, newValue), class One is initialized.
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ClassInitFieldSetDoubleStatic.java
 *- @ExecuteClass: ClassInitFieldSetDoubleStatic
 *- @ExecuteArgs:
 */

import java.lang.annotation.*;
import java.lang.reflect.Field;

public class ClassInitFieldSetDoubleStatic {
    static StringBuffer result = new StringBuffer("");

    public static void main(String[] args) {
        try {
            Class clazz = Class.forName("One", false, One.class.getClassLoader());
            Field f = clazz.getField("hiDouble");
            if (result.toString().compareTo("") == 0) {
                f.setDouble(null, 0.2589);
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
        ClassInitFieldSetDoubleStatic.result.append("Super");
    }
}

interface InterfaceSuper {
    String a = ClassInitFieldSetDoubleStatic.result.append("|InterfaceSuper|").toString();
}

@A(i = 1)
class One extends Super implements InterfaceSuper {
    static {
        ClassInitFieldSetDoubleStatic.result.append("One");
    }

    public static double hiDouble = 4.5;
}

@Target(ElementType.TYPE)
@Retention(RetentionPolicy.RUNTIME)
@interface A {
    int i() default 0;
    String a = ClassInitFieldSetDoubleStatic.result.append("|InterfaceA|").toString();
}

class Two extends One {
    static {
        ClassInitFieldSetDoubleStatic.result.append("Two");
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n