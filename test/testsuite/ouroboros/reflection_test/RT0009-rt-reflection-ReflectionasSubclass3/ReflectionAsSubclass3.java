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
 * -@TestCaseID: ReflectionAsSubclass3
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionAsSubclass3.java
 *- @Title/Destination: class A calls Class.asSubclass on class B, if A is not a subclass of B, throws ClassCastException
 *- @Brief:no:
 * -#step1: 通过反射C_1类，并对其返回值以D.class为参数调用asSubclass()方法；
 * -#step2: 同理，通过反射C_2类，并对其返回值以D.class为参数调用asSubclass()方法；
 * -#step3: 通过反射D_1类，并对其返回值以C.class为参数调用asSubclass()方法；
 * -#step4: step1、step2、step3中方法均调用失败，即无法获取到对应类的子类，会抛出ClassCastException；
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionAsSubclass3.java
 *- @ExecuteClass: ReflectionAsSubclass3
 *- @ExecuteArgs:
 */

interface C {
}

class C_1 implements C {
}

class C_2 extends C_1 {
}

class D {
}

class D_1 extends D {
}

public class ReflectionAsSubclass3 {
    public static void main(String[] args) {
        try {
            Class.forName("C_1").asSubclass(D.class);
        } catch (ClassCastException e1) {
            try {
                Class.forName("C_2").asSubclass(D.class);
            } catch (ClassCastException e2) {
                try {
                    Class.forName("D_1").asSubclass(C.class);
                } catch (ClassCastException e3) {
                    System.out.println(0);
                } catch (ClassNotFoundException e4) {
                    System.out.println(2);
                }
            } catch (ClassNotFoundException e5) {
                System.out.println(2);
            }
        } catch (ClassNotFoundException e6) {
            System.out.println(2);
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n