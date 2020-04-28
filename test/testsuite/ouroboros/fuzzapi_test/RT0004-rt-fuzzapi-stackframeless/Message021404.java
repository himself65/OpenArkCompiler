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
 * -@TestCaseID: Message021404.java
 *- @Title/Destination: Unsafe api: getUnsafe().when invoke use reflect,stack frame will miss one
 *- @Brief:
 *- @Expect:expected.txt
 *- @Priority: High
 *- @Source: Message021404.java
 *- @ExecuteClass: Message021404
 *- @ExecuteArgs:
 */


import sun.misc.Unsafe;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class Message021404 {
    private static int res = 99;
    public static void main(String[] args) {
        try {
            System.out.println(run());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private static int run() throws NoSuchFieldException, IllegalAccessException, NoSuchMethodException, InvocationTargetException {
        int result;
        result = message021404();
        if (result == 4 && Message021404.res == 89) {
            result = 0;
        }
        return result;
    }

    private static int message021404() throws NoSuchFieldException, IllegalAccessException, NoSuchMethodException, InvocationTargetException {
        Field f = Unsafe.class.getDeclaredField("theUnsafe");
        f.setAccessible(true);
        Unsafe s = (Unsafe)f.get(null);
        Method m = Unsafe.class.getDeclaredMethod("getUnsafe");
        m.invoke(s);
        Message021404.res -= 10;
        return 4;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0