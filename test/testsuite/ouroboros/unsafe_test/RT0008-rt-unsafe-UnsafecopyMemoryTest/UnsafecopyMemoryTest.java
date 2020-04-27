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
 * -@TestCaseID: UnsafecopyMemoryTest
 * -@TestCaseName: Unsafe api:  copyMemory(long srcAddr, long dstAddr, long bytes)
 * -@TestCaseType: Function Test
 * -@RequirementName: VMRuntime_registerNativeAllocation接口实现
 * -@Brief:
 * -#step1:allocateMemory to put value
 * -#step2:copyMemory from step1
 * -#step3:get value from step2,should be same as step1 put
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: UnsafecopyMemoryTest.java
 * -@ExecuteClass: UnsafecopyMemoryTest
 * -@ExecuteArgs:
 */

import sun.misc.Unsafe;

import java.io.PrintStream;
import java.lang.reflect.Field;
import java.lang.reflect.Method;

public class UnsafecopyMemoryTest {
    private static int res = 99;
    public static void main(String[] args) {
        System.out.println(run(args, System.out));
    }

    private static int run(String[] args, PrintStream out) {
        int result = 2/*STATUS_FAILED*/;
        try {
            result = UnsafecopyMemoryTest_1();
        } catch (Exception e) {
            e.printStackTrace();
            UnsafecopyMemoryTest.res -= 2;
        }

        if (result == 3 && UnsafecopyMemoryTest.res == 97) {
            result =0;
        }
        return result;
    }

    private static int UnsafecopyMemoryTest_1() {
        Unsafe unsafe;
        Field field;
        long address1;
        long address2;
        byte result;
        Method m;
        try {
            field = Unsafe.class.getDeclaredField("theUnsafe");
            field.setAccessible(true);
            unsafe = (Unsafe)field.get(null);
            address1 = unsafe.allocateMemory(5);
            byte b = 101;
            unsafe.putByte(address1, b);
            address2 = unsafe.allocateMemory(5);
            unsafe.copyMemory(address1, address2, 5);
//            System.out.println(Arrays.toString(bytes));
            result = unsafe.getByte(address2);
//            System.out.println(result);
            if (result == 101) {
                UnsafecopyMemoryTest.res -= 2;
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