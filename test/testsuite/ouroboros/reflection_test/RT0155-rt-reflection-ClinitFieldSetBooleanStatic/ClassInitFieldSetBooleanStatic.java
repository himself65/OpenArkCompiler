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
 * -@TestCaseID: ClassInitFieldSetBooleanStatic
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ClassInitFieldSetBooleanStatic.java
 *- @Title/Destination: When f is a static field of class One and call f.setBoolean(), class One is initialized.
 *- @Brief:no:
 * -#step1: Class.forName("One" , false, One.class.getClassLoader()) and clazz.getField to get a static field f of class
 *          One.
 * -#step2: Call method f.setBoolean(null, newValue), class One is initialized.
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ClassInitFieldSetBooleanStatic.java
 *- @ExecuteClass: ClassInitFieldSetBooleanStatic
 *- @ExecuteArgs:
 */

import java.lang.annotation.*;
import java.lang.reflect.Field;

public class ClassInitFieldSetBooleanStatic {
    static StringBuffer result = new StringBuffer("");

    public static void main(String[] args) {
        try {
            Class clazz = Class.forName("One", false, One.class.getClassLoader());
            Field f = clazz.getField("hiBoolean");
            if (result.toString().compareTo("") == 0) {
                f.setBoolean(null, false);
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
        ClassInitFieldSetBooleanStatic.result.append("Super");
    }
}

interface InterfaceSuper {
    String a = ClassInitFieldSetBooleanStatic.result.append("|InterfaceSuper|").toString();
}

@A(i = 1)
class One extends Super implements InterfaceSuper {
    static {
        ClassInitFieldSetBooleanStatic.result.append("One");
    }

    String what = "lala";
    public static boolean hiBoolean = false;
    One() {
    }

    public One(String s) {
        what = s;
        System.out.println(s);
    }

    public int testOne(String a) {
        System.out.println(a);
        return 0;
    }
}

@Target(ElementType.TYPE)
@Retention(RetentionPolicy.RUNTIME)
@interface A {
    int i() default 0;
    String a = ClassInitFieldSetBooleanStatic.result.append("|InterfaceA|").toString();
}

class Two extends One {
    static {
        ClassInitFieldSetBooleanStatic.result.append("Two");
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n