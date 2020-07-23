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
 * -@TestCaseID:Alloc_36_40x8B
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title:ROS Allocator is in charge of applying and releasing objects.This testcase is mainly for testing objects from 36*8B to 40*8B(max)
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: Alloc_36_40x8B.java
 *- @ExecuteClass: Alloc_36_40x8B
 *- @ExecuteArgs:
 */

import java.util.ArrayList;

public class Alloc_36_40x8B {
    private final static int PAGE_SIZE = 4 * 1024;
    private final static int OBJ_HEADSIZE = 8;
    private final static int MAX_36_8B = 36 * 8;
    private final static int MAX_37_8B = 37 * 8;
    private final static int MAX_38_8B = 38 * 8;
    private final static int MAX_39_8B = 39 * 8;
    private final static int MAX_40_8B = 40 * 8;
    private static ArrayList<byte[]> store;

    private static int alloc_test(int slot_type) {
        store = new ArrayList<byte[]>();
        byte[] temp;
        int i;
        if (slot_type == 24) {
            i = 1;
        } else if (slot_type == 1024) {
            i = 64 * 8 + 1 - OBJ_HEADSIZE;
        } else {
            i = slot_type - 2 * 8 + 1;
        }

        for (; i <= slot_type - OBJ_HEADSIZE; i++) {
            for (int j = 0; j < (PAGE_SIZE * 4 / (i + OBJ_HEADSIZE) + 10); j++) {
                temp = new byte[i];
                store.add(temp);
            }
        }
        int check_size = store.size();
        store = new ArrayList<byte[]>();
        return check_size;
    }

    public static void main(String[] args) {
        store = new ArrayList<byte[]>();
        int countSize36 = alloc_test(MAX_36_8B);
        int countSize37 = alloc_test(MAX_37_8B);
        int countSize38 = alloc_test(MAX_38_8B);
        int countSize39 = alloc_test(MAX_39_8B);
        int countSize40 = alloc_test(MAX_40_8B);
        //System.out.println(countSize36);
        //System.out.println(countSize37);
        //System.out.println(countSize38);
        //System.out.println(countSize39);
        //System.out.println(countSize40);

        if (countSize36 == 537 && countSize37 == 524 && countSize38 == 512 && countSize39 == 501 && countSize40 == 491)
            System.out.println("ExpectResult");
        else
            System.out.println("Error");
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\n