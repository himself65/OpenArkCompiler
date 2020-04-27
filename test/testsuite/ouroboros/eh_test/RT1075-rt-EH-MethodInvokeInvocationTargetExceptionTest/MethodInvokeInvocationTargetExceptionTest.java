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
 * -@TestCaseID: MethodInvokeInvocationTargetExceptionTest
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1: Create a Method method.
 * -#step2: Generate an instance object of the method and assign parameters.
 * -#step3: Call the method's invoke method.
 * -#step4: Check the InvocationTargetException thrown by invoke Method.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: MethodInvokeInvocationTargetExceptionTest.java
 * -@ExecuteClass: MethodInvokeInvocationTargetExceptionTest
 * -@ExecuteArgs:
 */

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class MethodInvokeInvocationTargetExceptionTest {
    public static void main(String[] args) {
        try {
            invokeMethod();
        } catch (InvocationTargetException e) {
            System.out.println(0);
        } catch (Exception e) {
            System.out.println(3);
        }
    }

    static void invokeMethod()
            throws NoSuchMethodException, ClassNotFoundException, IllegalAccessException, InvocationTargetException {
        Method method = Class.forName("java.lang.Class").getDeclaredMethod("getConstructor", Class[].class);
        Object instance = Class.forName("java.security.CodeSigner");
        Object[] parameters = new Object[1];

        parameters[0] =
                new Class[]{
                        java.security.CodeSigner.class,
                        java.util.concurrent.locks.AbstractOwnableSynchronizer.class,
                        java.util.concurrent.locks.AbstractOwnableSynchronizer.class,
                        java.util.LinkedHashMap.class,
                        java.nio.channels.AsynchronousFileChannel.class,
                        java.sql.Time.class,
                        java.util.EnumMap.class,
                        java.nio.channels.FileLock.class,
                        java.util.WeakHashMap.class
                };

        method.invoke(instance, parameters);
        System.out.println(2);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n
