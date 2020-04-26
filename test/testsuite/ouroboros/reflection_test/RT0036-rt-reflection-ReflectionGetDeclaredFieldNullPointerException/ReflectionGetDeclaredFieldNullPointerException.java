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
 * -@TestCaseID: ReflectionGetDeclaredFieldNullPointerException.
 *- @RequirementName: Java Reflection
 *- @TestCaseName:ReflectionGetDeclaredFieldNullPointerException.java
 *- @Title/Destination: Class.GetDeclaredField(null) throws NullPointerException.
 *- @Brief:no:
 * -#step1: Create two test class.
 * -#step2: Use classloader to load class.
 * -#step3: Test Class.GetDeclaredField() with null parameter.
 * -#step4: Check that NullPointerException was threw.
 *- @Expect: 0\n
 *- @Priority: High
 *- @Source: ReflectionGetDeclaredFieldNullPointerException.java
 *- @ExecuteClass: ReflectionGetDeclaredFieldNullPointerException
 *- @ExecuteArgs:
 */

import java.lang.reflect.Field;

class GetDeclaredField2_a {
    public int i_a = 5;
    String s_a = "bbb";
}

class GetDeclaredField2 extends GetDeclaredField2_a {
    public int i = 1;
    String s = "aaa";
    private double d = 2.5;
    protected float f = -222;
}

public class ReflectionGetDeclaredFieldNullPointerException {
    public static void main(String[] args) {
        int result = 0; /* STATUS_Success */
        try {
            Class zqp = Class.forName("GetDeclaredField2");
            Field zhu1 = zqp.getDeclaredField("i_a");
            result = -1;
        } catch (ClassNotFoundException e1) {
            System.err.println(e1);
            result = -1;
        } catch (NullPointerException e2) {
            System.err.println(e2);
            result = -1;
        } catch (NoSuchFieldException e3) {
            try {
                Class zqp = Class.forName("GetDeclaredField2");
                Field zhu1 = zqp.getDeclaredField(null);
                result = -1;
            } catch (ClassNotFoundException e4) {
                System.err.println(e4);
                result = -1;
            } catch (NoSuchFieldException e5) {
                System.err.println(e5);
                result = -1;
            } catch (NullPointerException e6) {
                result = 0;
            }
        }
        System.out.println(result);
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n