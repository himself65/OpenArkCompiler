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
 * -@TestCaseID:Alloc_3_10x8B
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title:ROS Allocator is in charge of applying and releasing objects.This testcase is mainly for testing objects from 3*8B to 10*8B(max)
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: Alloc_3_10x8B.java
 *- @ExecuteClass: Alloc_3_10x8B
 *- @ExecuteArgs:
 */

import java.util.ArrayList;

public class Alloc_3_10x8B {
    private final static int PAGE_SIZE = 4 * 1024;
    private final static int OBJ_HEADSIZE = 8;
    private final static int MAX_3_8B = 3 * 8;
    private final static int MAX_4_8B = 4 * 8;
    private final static int MAX_5_8B = 5 * 8;
    private final static int MAX_6_8B = 6 * 8;
    private final static int MAX_7_8B = 7 * 8;
    private final static int MAX_8_8B = 8 * 8;
    private final static int MAX_9_8B = 9 * 8;
    private final static int MAX_10_8B = 10 * 8;
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
            for (int j = 0; j < (PAGE_SIZE / (i + OBJ_HEADSIZE) + 10); j++) {
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
        int countSize3 = alloc_test(MAX_3_8B);
        int countSize4 = alloc_test(MAX_4_8B);
        int countSize5 = alloc_test(MAX_5_8B);
        int countSize6 = alloc_test(MAX_6_8B);
        int countSize7 = alloc_test(MAX_7_8B);
        int countSize8 = alloc_test(MAX_8_8B);
        int countSize9 = alloc_test(MAX_9_8B);
        int countSize10 = alloc_test(MAX_10_8B);
        if (countSize3 == 4488 && countSize4 == 1234 && countSize5 == 978 && countSize6 == 816 && countSize7 == 701 && countSize8 == 620 && countSize9 == 556 && countSize10 == 505)
            System.out.println("ExpectResult");
        else
            System.out.println("Error");
    }

}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n