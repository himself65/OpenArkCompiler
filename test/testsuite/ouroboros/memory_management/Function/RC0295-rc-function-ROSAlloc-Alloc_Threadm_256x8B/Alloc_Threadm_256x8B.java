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
 * -@TestCaseID:Alloc_Threadm_256x8B
 * -@TestCaseName:MyselfClassName
 * -@RequirementName:[运行时需求]支持自动内存管理
 * -@Title:ROS Allocator is in charge of applying and releasing objects.This mulit thread testcase is mainly for testing
 *         objects from 192*8B to 256*8B(max) with 6 threads
 * -@Brief:functionTest
 * -@Expect:ExpectResult\n
 * -@Priority: High
 * -@Source: Alloc_Threadm_256x8B.java
 * -@ExecuteClass: Alloc_Threadm_256x8B
 * -@ExecuteArgs:
 */

import java.util.ArrayList;

class Alloc_Threadm_256x8B_01 extends Thread {
    private static final int PAGE_SIZE = 4 * 1024;
    private static final int OBJ_HEADSIZE = 8;
    private static final int MAX_256_8B = 256 * 8;
    private static boolean checkOut = false;

    public void run() {
        ArrayList<byte[]> store = new ArrayList<byte[]>();
        byte[] temp;
        int checkSize = 0;
        for (int i = 192 * 8 - OBJ_HEADSIZE; i <= MAX_256_8B - OBJ_HEADSIZE; i = i + 800) {
            for (int j = 0; j < (PAGE_SIZE * 512 / (i + OBJ_HEADSIZE) + 10); j++) {
                temp = new byte[i];
                store.add(temp);
                checkSize++;
                store = new ArrayList<byte[]>();
            }
        }
        if (checkSize == 1375) {
            checkOut = true;
        }
    }

    public boolean check() {
        return checkOut;
    }

}

public class Alloc_Threadm_256x8B {
    public static void main(String[] args) {
        Runtime.getRuntime().gc();
        Alloc_Threadm_256x8B_01 test1 = new Alloc_Threadm_256x8B_01();
        test1.start();
        Alloc_Threadm_256x8B_01 test2 = new Alloc_Threadm_256x8B_01();
        test2.start();
        Alloc_Threadm_256x8B_01 test3 = new Alloc_Threadm_256x8B_01();
        test3.start();
        Alloc_Threadm_256x8B_01 test4 = new Alloc_Threadm_256x8B_01();
        test4.start();
        try {
            test1.join();
            test2.join();
            test3.join();
            test4.join();
        } catch (InterruptedException e) {
            // do nothing
        }
        if (test1.check() && test2.check() && test3.check() && test4.check()) {
            System.out.println("ExpectResult");
        } else {
            System.out.println("Error");
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n