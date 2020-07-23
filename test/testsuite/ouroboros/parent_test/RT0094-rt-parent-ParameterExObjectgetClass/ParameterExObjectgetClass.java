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
 * -@TestCaseID: ParameterExObjectgetClass.java
 * -@TestCaseName: Exception in reflect Parameter:  final Class<?> getClass()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ParameterExObjectgetClass.java
 * -@ExecuteClass: ParameterExObjectgetClass
 * -@ExecuteArgs:
 */

import java.lang.reflect.Method;
import java.lang.reflect.Parameter;

public class ParameterExObjectgetClass {
    static int res = 99;

    public static void main(String argv[]) {
        System.out.println(new ParameterExObjectgetClass().run());
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = parameterExObjectgetClass1();
        } catch (Exception e) {
            System.out.println(e);
            ParameterExObjectgetClass.res = ParameterExObjectgetClass.res - 20;
        }

        if (result == 4 && ParameterExObjectgetClass.res == 89) {
            result = 0;
        }

        return result;
    }

    private int parameterExObjectgetClass1() throws NoSuchMethodException, SecurityException {
        int result1 = 4; /*STATUS_FAILED*/
        //  final Class<?> getClass()
        // System.out.println("-0-");
        Method method = Parameter01a.class.getMethod("getName", new Class[]{String.class});
        // System.out.println("-1-");
        Parameter[] parameters = method.getParameters();
        // System.out.println("-2-");
        Class px1 = parameters[0].getClass();
        // System.out.println(px1);
        if (px1.toString().equals("class java.lang.reflect.Parameter")) {
            ParameterExObjectgetClass.res = ParameterExObjectgetClass.res - 10;
        }
        return result1;
    }

}

class Parameter01a {
    String name = "default";

    public String getName(String str1) {
        return this.name;
    }

}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n