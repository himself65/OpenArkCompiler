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
 * -@TestCaseID: UnsafesetMemoryTest
 * -@TestCaseName: Unsafe api: setMemory()
 * -@TestCaseType: Function Test
 * -@RequirementName: VMRuntime_registerNativeAllocation接口实现
 * -@Brief:
 * -#step1:allocateMemory for one Field
 * -#step2:set value at step1 by Unsafe.setMemory
 * -#step3:check value after ste2 correct
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: UnsafesetMemoryTest.java
 * -@ExecuteClass: UnsafesetMemoryTest
 * -@ExecuteArgs:
 */

import sun.misc.Unsafe;

import java.io.PrintStream;
import java.lang.reflect.Field;

public class UnsafesetMemoryTest {
    private static int res = 99;
    public static void main(String[] args) {
        System.out.println(run(args, System.out));
    }

    private static int run(String[] args, PrintStream out) {
        int result = 2/*STATUS_FAILED*/;
        try {
            result = UnsafesetMemoryTest_1();
        } catch (Exception e) {
            e.printStackTrace();
            UnsafesetMemoryTest.res -= 2;
        }

        if (result == 3 && UnsafesetMemoryTest.res == 97) {
            result =0;
        }
        return result;
    }

    private static int UnsafesetMemoryTest_1() {
        Unsafe unsafe;
        Field field;
        long address;
        byte result;
        try {
            field = Unsafe.class.getDeclaredField("theUnsafe");
            field.setAccessible(true);
            unsafe = (Unsafe)field.get(null);
            address = unsafe.allocateMemory(4);
//            System.out.println(address);
            byte b = 127;
            unsafe.setMemory(address, 4L, b);
            result = unsafe.getByte(address);
//            System.out.println(result);
            if (result == b) {
                UnsafesetMemoryTest.res -= 2;
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

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n