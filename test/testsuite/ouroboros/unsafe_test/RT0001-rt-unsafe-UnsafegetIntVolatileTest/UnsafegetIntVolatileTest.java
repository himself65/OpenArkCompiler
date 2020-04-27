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
 * -@TestCaseID: UnsafegetIntVolatileTest
 * -@TestCaseName: Unsafe api: getIntvolatile()
 * -@TestCaseType: Function Test
 * -@RequirementName: VMRuntime_registerNativeAllocation接口实现
 * -@Brief:
 * -#step1:getobjectFieldOffset
 * -#step2:Unsafe.getIntVolatile of step1
 * -#step3:get result as step1 set
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: UnsafegetIntVolatileTest.java
 * -@ExecuteClass: UnsafegetIntVolatileTest
 * -@ExecuteArgs:
 */

import sun.misc.Unsafe;

import java.io.PrintStream;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.TreeSet;

public class UnsafegetIntVolatileTest {
    private static int res = 99;
    public static void main(String[] args) {
        System.out.println(run(args, System.out));
    }

    private static int run(String[] args, PrintStream out) {
        int result = 2/*STATUS_FAILED*/;
        try {
            result = UnsafegetIntVolatileTest_1();
        } catch (Exception e) {
            e.printStackTrace();
            UnsafegetIntVolatileTest.res -= 2;
        }

        if (result == 3 && UnsafegetIntVolatileTest.res == 97) {
            result =0;
        }
        return result;
    }


    private static int UnsafegetIntVolatileTest_1()  {
        Unsafe unsafe;
        Field field;
        Long offset;
        Field param;
        Object obj;
        int result;
        try {
            field = Unsafe.class.getDeclaredField("theUnsafe");
            field.setAccessible(true);
            unsafe = (Unsafe)field.get(null);
            param = Billie6.class.getDeclaredField("age");
            offset = unsafe.objectFieldOffset(param);
            obj = new Billie6();
            result = unsafe.getIntVolatile(obj, offset);
            if (result == 18) {
                UnsafegetIntVolatileTest.res -= 2;
            }
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
            return 40;
        } catch (IllegalAccessException e) {
            e.printStackTrace();
            return 41;
        }
        return 3;
    }
}
class Billie6 {
    public int height = 8;
    private String[] color = {"black","white"};
    private String owner = "Me";
    private byte length = 0x7;
    private String[] water = {"day","wet"};
    private long weight = 100L;
    private volatile int age = 18;
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n