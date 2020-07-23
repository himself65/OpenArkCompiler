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
 * -@TestCaseID: UnsafeunparkTest
 * -@TestCaseName: Unsafe api: unpark()
 * -@TestCaseType: Function Test
 * -@RequirementName: VMRuntime_registerNativeAllocation接口实现
 * -@Brief:
 * -#step1:threa1 park
 * -#step2:thread2 to unpark thread1
 * -#step3:thread1 go on excute
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: UnsafeunparkTest.java
 * -@ExecuteClass: UnsafeunparkTest
 * -@ExecuteArgs:
 */

import sun.misc.Unsafe;

import java.io.PrintStream;
import java.lang.reflect.Field;

public class UnsafeunparkTest {
    private static int res = 99;
    private static int flag = 0;
    public static void main(String[] args) throws InterruptedException {
        System.out.println(run(args, System.out));
    }

    private static int run(String[] args, PrintStream out) throws InterruptedException {
        int result = 2/*STATUS_FAILED*/;
        try {
            result = UnsafeunparkTest_1();
        } catch (Exception e) {
            e.printStackTrace();
            UnsafeunparkTest.res -= 2;
        }
        Thread.sleep(4000);
//        System.out.println("===================="+flag);
        if (result == 3 && UnsafeunparkTest.flag == 1214210) {
            result =0;
        }
        return result;
    }

    private static int UnsafeunparkTest_1() {
        int result = 3;
        Field field;
        try {
            field = Unsafe.class.getDeclaredField("theUnsafe");
            field.setAccessible(true);
            Unsafe unsafe = (Unsafe)field.get(null);
            Thread boyThread = new Thread(new Runnable() {
                @Override
                public void run() {
//                    System.out.println("boy: 我要吃鸡");
                    flag += (flag+1)*10;
//                    System.out.println(flag);
                    unsafe.park(false,0);
//                    System.out.println("boy: park1");
                    flag += (flag+3)*1000;
//                    System.out.println(flag);
//                    unsafe.park(false,0); // 第二次会阻塞住，因为只有一个permit
//                    System.out.println("boy: park2");
//                    System.out.println("boy: 不会再执行了");
                }
            });

            Thread girlThread = new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        Thread.sleep(3000);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
//                    System.out.println("girl: OK");
                    flag += (flag+2)*100;
//                    System.out.println(flag);
                    unsafe.unpark(boyThread); // unpark两次，但是permit不会叠加
                    unsafe.unpark(boyThread);
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


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n