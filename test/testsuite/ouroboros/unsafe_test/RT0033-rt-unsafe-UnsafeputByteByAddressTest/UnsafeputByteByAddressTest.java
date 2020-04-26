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
 * -@TestCaseID: UnsafeputByteByAddressTest
 * -@TestCaseName: Unsafe api: putByte(Address)
 * -@TestCaseType: Function Test
 * -@RequirementName: VMRuntime_registerNativeAllocation接口实现
 * -@Brief:
 * -#step1:allocateMemory for one Field of Byte
 * -#step2:set value of step2 by Unsafe.putByte
 * -#step3:check value after step3 correct
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: UnsafeputByteByAddressTest.java
 * -@ExecuteClass: UnsafeputByteByAddressTest
 * -@ExecuteArgs:
 */

import sun.misc.Unsafe;

import java.io.PrintStream;
import java.lang.reflect.Field;

public class UnsafeputByteByAddressTest {
    private static int res = 99;
    public static void main(String[] args) {
        System.out.println(run(args, System.out));
    }

    private static int run(String[] args, PrintStream out) {
        int result = 2/*STATUS_FAILED*/;
        try {
            result = UnsafeputByteByAddressTest_1();
        } catch (Exception e) {
            e.printStackTrace();
            UnsafeputByteByAddressTest.res -= 2;
        }

        if (result == 3 && UnsafeputByteByAddressTest.res == 97) {
            result =0;
        }
        return result;
    }

    private static int UnsafeputByteByAddressTest_1() {
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
            unsafe.putByte(address, b);
            result = unsafe.getByte(address);
//            System.out.println(result);
            if (result == b) {
                UnsafeputByteByAddressTest.res -= 2;
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