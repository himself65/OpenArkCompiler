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
 * -@TestCaseID: UnsafecompareAndSwapLongTest
 * -@TestCaseName: Unsafe api: compareAndSwapLong()
 * -@TestCaseType: Function Test
 * -@RequirementName: VMRuntime_registerNativeAllocation接口实现
 * -@Brief:
 * -#step1:Prepare Field weight = 100L(long type)
 * -#step2:invoke Unsafe.compareAndSwapLong to swap value of step1
 * -#step3:swap success
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: UnsafecompareAndSwapLongTest.java
 * -@ExecuteClass: UnsafecompareAndSwapLongTest
 * -@ExecuteArgs:
 */

import sun.misc.Unsafe;

import java.io.PrintStream;
import java.lang.reflect.Field;

public class UnsafecompareAndSwapLongTest {
    private static int res = 99;
    public static void main(String[] args) {
        System.out.println(run(args, System.out));
    }

    private static int run(String[] args, PrintStream out) {
        int result = 2/*STATUS_FAILED*/;
        try {
            result = UnsafecompareAndSwapLongTest_1();
        } catch (Exception e) {
            e.printStackTrace();
            UnsafecompareAndSwapLongTest.res = UnsafecompareAndSwapLongTest.res - 10;
        }
//        System.out.println(result);
//        System.out.println(UnsafearrayBaseOffsetTest.res);
        if (result == 3 && UnsafecompareAndSwapLongTest.res == 97) {
            result =0;
        }
        return result;
    }

    private static int UnsafecompareAndSwapLongTest_1() {
        Unsafe unsafe;
        Field field;
        Field param;
        Object obj;
        long offset;
        long result;
        try {
            field = Unsafe.class.getDeclaredField("theUnsafe");
            field.setAccessible(true);
            unsafe = (Unsafe)field.get(null);
            obj = new Billie4();
            param = Billie4.class.getDeclaredField("weight");
            offset = unsafe.objectFieldOffset(param);
            unsafe.compareAndSwapLong(obj, offset, 100L, 200L);
            result = unsafe.getLong(obj, offset);
            if (result == 200L) {
                UnsafecompareAndSwapLongTest.res -= 2;
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

class Billie4 {
    public int height = 8;
    private String[] color = {"black","white"};
    private String owner = "Me";
    private byte length = 0x7;
    private String[] water = {"day","wet"};
    private long weight = 100L;
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n