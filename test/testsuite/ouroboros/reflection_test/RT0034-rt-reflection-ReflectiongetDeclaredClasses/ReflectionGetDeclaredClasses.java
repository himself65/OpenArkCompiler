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
 * -@TestCaseID: ReflectionGetDeclaredClasses
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionGetDeclaredClasses.java
 *- @Title/Destination: Class.getDeclaredClasses() result an array of class objects that represent all declared members
 *                      of this class,includes public, protected, default (package) access, and private classes and
 *                      interfaces,excludes inherited classes and interfaces
 *- @Brief:no:
 * -#step1: Create a class include inner class.
 * -#step2: Use classloader to load class.
 * -#step3: Return an array of class objects by calling Class.getDeclaredClasses().
 * -#step4: Check that the array of class objects exclude inherited classes and interfaces.
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionGetDeclaredClasses.java
 *- @ExecuteClass: ReflectionGetDeclaredClasses
 *- @ExecuteArgs:
 */

class ReflectiongetDeclaredClasses_a {
    public class getDeclaredClasses_a1 {
    }

    private class getDeclaredClasses_a2 {
    }

    protected class getDeclaredClasses_a3 {
    }
}

public class ReflectionGetDeclaredClasses extends ReflectiongetDeclaredClasses_a {
    public static void main(String[] args) {
        int result = 0; /* STATUS_Success */
        try {
            Class zqp = Class.forName("ReflectionGetDeclaredClasses");
            Class<?>[] j = zqp.getDeclaredClasses();
            if (j.length == 3) {
                for (int i = 0; i < j.length; i++) {
                    if (j[i].getName().indexOf("getDeclaredClasses_a") != -1) {
                        result = -1;
                    }
                }
            }
        } catch (ClassNotFoundException e) {
            System.err.println(e);
            result = -1;
        }
        System.out.println(result);
    }

    public class getDeclaredClassestest1 {
    }

    private class getDeclaredClassestest2 {
    }

    protected class getDeclaredClassestest3 {
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n