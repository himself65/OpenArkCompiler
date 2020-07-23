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
 * -@TestCaseID: Maple_MemoryManagement2.0_UnsafeTest01
 *- @TestCaseName: UnsafeTest01
 *- @TestCaseType: Function Testing for MemoryBindingMethod Test
 *- @RequirementName: 运行时支持GCOnly
 *- @Brief:unsafe.compareAndSwapObject()函数专项测试：验证compareAndSwapObject()方法的基本功能正常。
 *  -#step1:
 *- @Expect:0\n
 *- @Priority: High
 *- @Source:UnsafeTest01.java
 *- @ExecuteClass:UnsafeTest01
 *- @ExecuteArgs:
 */

import sun.misc.Unsafe;

import java.io.PrintStream;
import java.lang.reflect.Field;

public class UnsafeTest01 {
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
            UnsafeTest01.res = UnsafeTest01.res - 10;
        }
        if (result == 3 && UnsafeTest01.res == 97) {
            result = 0;
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
            unsafe = (Unsafe) field.get(null);
            obj = new Billie5();
            param = Billie5.class.getDeclaredField("owner");
            offset = unsafe.objectFieldOffset(param);
            unsafe.compareAndSwapObject(obj, offset, "Me", "billie5");
            result = unsafe.getObject(obj, offset);
            if (result.equals("billie5")) {
                UnsafeTest01.res -= 2;
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
    private String[] color = {"black", "white"};
    private String owner = "Me";
    private byte length = 0x7;
    private String[] water = {"day", "wet"};
    private long weight = 100L;
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n