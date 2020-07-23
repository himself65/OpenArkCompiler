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
 * -@TestCaseID: UnsafecompareAndSwapObjectTest
 * -@TestCaseName: Unsafe api: compareAndSwapObject()
 * -@TestCaseType: Function Test
 * -@RequirementName: VMRuntime_registerNativeAllocation接口实现
 * -@Brief:
 * -#step1:Prepare Field owner = "Me"(Object)
 * -#step2:invoke Unsafe.compareAndSwapObject to swap value of step1
 * -#step3:swap success
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: UnsafecompareAndSwapObjectTest.java
 * -@ExecuteClass: UnsafecompareAndSwapObjectTest
 * -@ExecuteArgs:
 */

import sun.misc.Unsafe;

import java.io.PrintStream;
import java.lang.reflect.Field;

public class UnsafecompareAndSwapObjectTest {
    private static int res = 99;
    public static void main(String[] args) {
        System.out.println(run(args, System.out));
    }

    private static int run(String[] args, PrintStream out) {
        int result = 2/*STATUS_FAILED*/;
        try {
            result = UnsafecompareAndSwapObjectTest_1();
        } catch (Exception e) {
            e.printStackTrace();
            UnsafecompareAndSwapObjectTest.res = UnsafecompareAndSwapObjectTest.res - 10;
        }
//        System.out.println(result);
//        System.out.println(UnsafearrayBaseOffsetTest.res);
        if (result == 3 && UnsafecompareAndSwapObjectTest.res == 97) {
            result =0;
        }
        return result;
    }

    private static int UnsafecompareAndSwapObjectTest_1() {
        Unsafe unsafe;
        Field field;
        Field param;
        Object obj;
        long offset;
        Object result;
        try {
            field = Unsafe.class.getDeclaredField("theUnsafe");
            field.setAccessible(true);
            unsafe = (Unsafe)field.get(null);
            obj = new Billie5();
            param = Billie5.class.getDeclaredField("owner");
            offset = unsafe.objectFieldOffset(param);
            unsafe.compareAndSwapObject(obj, offset, "Me", "billie5");
            result = unsafe.getObject(obj, offset);
            if (result.equals("billie5")) {
                UnsafecompareAndSwapObjectTest.res -= 2;
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

class Billie5 {
    public int height = 8;
    private String[] color = {"black","white"};
    private String owner = "Me";
    private byte length = 0x7;
    private String[] water = {"day","wet"};
    private long weight = 100L;
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n