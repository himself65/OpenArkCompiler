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
 * -@TestCaseID: UnsafegetAndSetLongTest
 * -@TestCaseName: Unsafe api: getAndSetLong()
 * -@TestCaseType: Function Test
 * -@RequirementName: VMRuntime_registerNativeAllocation接口实现
 * -@Brief:
 * -#step1:Prepare Field weight = 100L(Long type)
 * -#step2:invoke Unsafe.getAndSetLong to set another value
 * -#step3:check the Field,modify success
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: UnsafegetAndSetLongTest.java
 * -@ExecuteClass: UnsafegetAndSetLongTest
 * -@ExecuteArgs:
 */
import sun.misc.Unsafe;

import java.io.PrintStream;
import java.lang.reflect.Field;

public class UnsafegetAndSetLongTest {
    private static int res = 99;
    public static void main(String[] args) {
        System.out.println(run(args, System.out));
    }

    private static int run(String[] args, PrintStream out) {
        int result = 2/*STATUS_FAILED*/;
        try {
            result = UnsafegetAndSetLongTest_1();
        } catch (Exception e) {
            e.printStackTrace();
            UnsafegetAndSetLongTest.res -= 2;
        }

        if (result == 3 && UnsafegetAndSetLongTest.res == 97) {
            result =0;
        }
        return result;
    }

    private static int UnsafegetAndSetLongTest_1() {
        Unsafe unsafe;
        Field field;
        Long offset;
        Field param;
        Object obj;
        long result;
        long result2;
        try {
            field = Unsafe.class.getDeclaredField("theUnsafe");
            field.setAccessible(true);
            unsafe = (Unsafe)field.get(null);
            param = Billie38.class.getDeclaredField("weight");
            offset = unsafe.objectFieldOffset(param);
            obj = new Billie38();
            result = unsafe.getAndSetLong(obj, offset, 99L);
//            System.out.println(result);
            result2 = unsafe.getInt(obj, offset);
//            System.out.println(result2);
            if (result == 100 && result2 == 99) {
                UnsafegetAndSetLongTest.res -= 2;
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

class Billie38 {
    public int height = 8;
    private String[] color = {"black","white"};
    private String owner = "Me";
    private byte length = 0x7;
    private String[] water = {"day","wet"};
    private long weight = 100L;
    private volatile int age = 18;
    private volatile long birth = 20010214L;
    private volatile String lastname = "eilish";
    private boolean gender  = false;
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n