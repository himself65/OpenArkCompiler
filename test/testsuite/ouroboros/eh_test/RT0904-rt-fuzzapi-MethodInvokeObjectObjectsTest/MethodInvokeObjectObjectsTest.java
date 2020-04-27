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
 * -@TestCaseID: MethodInvokeObjectObjectsTest.java
 * -@TestCaseName: Test Object invoke(Object obj, Object... args) of Method, check Exception order.
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1：Create an instance of a method from class java.util.concurrent.ConcurrentHashMap.
 * -#step2: Create Parameter param1 as a StringBuffer, Create Parameter param2 as a Object[].
 * -#step3: Test the method invoke(Object obj, Object... args), and check the Exception is thrown correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: MethodInvokeObjectObjectsTest.java
 * -@ExecuteClass: MethodInvokeObjectObjectsTest
 * -@ExecuteArgs:
 */

import java.lang.reflect.*;
import java.nio.charset.Charset;
import java.util.*;

public class MethodInvokeObjectObjectsTest {
    /**
     * test class
     */
    public static Class<?> clazz;

    public static void main(String[] args) {
        try {
            System.out.println(run());
        } catch (Exception e) {
            System.out.println(e);
        }
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public static int run() {
        try {
            clazz = Class.forName("java.lang.reflect.Method");
            Method method = clazz.getMethod("invoke", Object.class, Object[].class);
            String fcn2 = "java.util.concurrent.ConcurrentHashMap";
            int mIndex6 = 1;
            Method[] methods2 = Class.forName(fcn2).getDeclaredMethods();
            Arrays.sort(methods2, new ConstructorComparator());
            Object instance = methods2[mIndex6];
            Constructor con = java.lang.StringBuffer.class.getDeclaredConstructor();
            Object param1 = con.newInstance();
            Object[] param2 = new Object[4];
            Constructor con0 = java.lang.StringBuffer.class.getDeclaredConstructor();
            param2[0] = con0.newInstance();
            Constructor con1 =
                    java.security.spec.MGF1ParameterSpec.class.getDeclaredConstructor(java.lang.String.class);
            param2[1] = con1.newInstance("abcdefghijklmnopqrstuvwxxzABCDEFGHIJKLMNOPQRSTUVWXYZ-9223372036854775551");
            Constructor con2 = java.util.Stack.class.getDeclaredConstructor();
            param2[2] = con2.newInstance();
            Constructor con3 =
                    java.security.spec.ECGenParameterSpec.class.getDeclaredConstructor(java.lang.String.class);
            param2[3] = con3.newInstance("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
            Object result = method.invoke(instance, param1, param2);
            System.out.println(result);
        } catch (Exception e) {
            if (e.toString().equals("java.lang.reflect.InvocationTargetException")
                    && e.getCause().getClass().getName().toString().equals("java.lang.IllegalArgumentException")) {
                return 0;
            }
        }
        return -1;
    }
}

class ConstructorComparator implements Comparator<Object> {
    @Override
    public int compare(Object c1, Object c2) {
        byte[] b1 = c1.toString().getBytes(Charset.defaultCharset());
        byte[] b2 = c2.toString().getBytes(Charset.defaultCharset());
        int len = (b1.length < b2.length ? b1.length : b2.length);
        for (int i = 0; i < len; i++) {
            if (b1[i] < b2[i]) {
                return 1;
            } else if (b1[i] > b2[i]) {
                return -1;
            }
        }
        if (b1.length > b2.length) {
            return -1;
        } else if (b1.length < b2.length) {
            return 1;
        }
        return 1;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0