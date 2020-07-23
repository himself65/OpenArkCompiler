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
 * -@TestCaseID: UnsafeputOrderedIntTest
 * -@TestCaseName: Unsafe api: putOrderedInt()
 * -@TestCaseType: Function Test
 * -@RequirementName: VMRuntime_registerNativeAllocation接口实现
 * -@Brief:
 * -#step1:prepare one Class and get Field of Int
 * -#step2:invoke Unsafe.getInt to visit this Field
 * -#step3:set value of step2 by Unsafe.putOrderedInt
 * -#step4:check value of step3 immediately,the value may not be changed as step3
 * -#step5:check value of step3 after seconds,the value should be changed as step3
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: UnsafeputOrderedIntTest.java
 * -@ExecuteClass: UnsafeputOrderedIntTest
 * -@ExecuteArgs:
 */
import sun.misc.Unsafe;

import java.io.PrintStream;
import java.lang.reflect.Field;

public class UnsafeputOrderedIntTest {
    private static int res = 99;
    private static Object obj = new Object();
    private static Long offset =0l;
    private static int flag =0;
    public static void main(String[] args) throws InterruptedException {
        System.out.println(run(args, System.out));
    }

    private static int run(String[] args, PrintStream out) throws InterruptedException {
        int result = 2/*STATUS_FAILED*/;
        try {
            result = UnsafeputOrderedIntTest_1();
        } catch (Exception e) {
            e.printStackTrace();
            UnsafeputOrderedIntTest.res -= 2;
        }
//        System.out.println(flag);
        if (flag==0) {
//            System.out.println("It is true");
        }
        Thread.sleep(4000);
//        System.out.println(flag);
//        System.out.println("result:" +result);
//        System.out.println("res: "+ res);
        if (result == 3 && UnsafeputOrderedIntTest.flag == 10) {
            result = 0;
        }
        return result;
    }

    private static int UnsafeputOrderedIntTest_1() {
        int result = 3;
        Field field;
        try {
            field = Unsafe.class.getDeclaredField("theUnsafe");
            field.setAccessible(true);
            Unsafe unsafe = (Unsafe)field.get(null);
            Thread boyThread = new Thread(new Runnable() {
                @Override
                public void run() {
                    Field param = null;
                    try {
                        param = Billie14.class.getDeclaredField("height");
                    } catch (NoSuchFieldException e) {
                        e.printStackTrace();
                    }
                    offset = unsafe.objectFieldOffset(param);
                    obj = new Billie14();
                    unsafe.putOrderedInt(obj, offset, 10);
                }
            });

            Thread girlThread = new Thread(new Runnable() {
                @Override
                public void run() {
                    int result;
                    flag = unsafe.getInt(obj,offset);
//                    System.out.println("======================="+flag);
                    try {
                        Thread.sleep(3000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    flag = unsafe.getInt(obj,offset);
//                    System.out.println("======================="+flag);
                }
            });
            boyThread.start();
            girlThread.start();
        } catch (Exception e) {
            e.printStackTrace();
            return 40;
        }
        return result;
    }
}

class Billie14 {
    public int height = 8;
    private String[] color = {"black","white"};
    private String owner = "Me";
    private byte length = 0x7;
    private String[] water = {"day","wet"};
    private long weight = 100L;
    private volatile int age = 18;
    private volatile long birth = 20010214L;
    private volatile String lastname = "eilish";
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n