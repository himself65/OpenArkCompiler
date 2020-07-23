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
 * -@TestCaseID:Alloc_Thread2_64x8B
 * -@TestCaseName:MyselfClassName
 * -@RequirementName:[运行时需求]支持自动内存管理
 * -@Title:ROS Allocator is in charge of applying and releasing objects.This mulit thread testcase is mainly for testing objects from 60*8B to 64*8B(max)
 * -@Brief:functionTest
 * -@Expect:ExpectResult\n
 * -@Priority: High
 * -@Source: Alloc_Thread2_64x8B.java
 * -@ExecuteClass: Alloc_Thread2_64x8B
 * -@ExecuteArgs:
 */

import java.util.ArrayList;

class Alloc_Thread2_64x8B_01 extends Thread {
    private static final int PAGE_SIZE = 4 * 1024;
    private static final int OBJ_HEADSIZE = 8;
    private static final int MAX_64_8B = 64 * 8;
    private static boolean checkout = false;

    public void run() {
        ArrayList<byte[]> store = new ArrayList<byte[]>();
        byte[] temp;
        for (int i = 480 - OBJ_HEADSIZE; i <= MAX_64_8B - OBJ_HEADSIZE; i++) {
            for (int j = 0; j < (PAGE_SIZE * 128 / (i + OBJ_HEADSIZE) + 10); j++) {
                temp = new byte[i];
                store.add(temp);
            }
        }
        int checkSize = store.size();
        if (checkSize == 35212) {
            checkout = true;
        }
    }

    public boolean check() {
        return checkout;
    }
}

public class Alloc_Thread2_64x8B {
    public static void main(String[] args) {
        Runtime.getRuntime().gc();
        Alloc_Thread2_64x8B_01 test1 = new Alloc_Thread2_64x8B_01();
        test1.start();
        Alloc_Thread2_64x8B_01 test2 = new Alloc_Thread2_64x8B_01();
        test2.start();
        try {
            test1.join();
            test2.join();
        } catch (InterruptedException e) {
            // do nothing
        }
        if (test1.check() && test2.check()) {
            System.out.println("ExpectResult");
        } else {
            System.out.println("Error");
        }
    }

}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n