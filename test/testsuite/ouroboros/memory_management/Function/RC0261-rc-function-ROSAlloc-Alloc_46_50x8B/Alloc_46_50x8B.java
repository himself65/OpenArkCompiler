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
 * -@TestCaseID:Alloc_46_50x8B
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title:ROS Allocator is in charge of applying and releasing objects.This testcase is mainly for testing objects from 46*8B to 50*8B(max)
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: Alloc_46_50x8B.java
 *- @ExecuteClass: Alloc_46_50x8B
 *- @ExecuteArgs:
 */

import java.util.ArrayList;

public class Alloc_46_50x8B {
    private final static int PAGE_SIZE = 4 * 1024;
    private final static int OBJ_HEADSIZE = 8;
    private final static int MAX_46_8B = 46 * 8;
    private final static int MAX_47_8B = 47 * 8;
    private final static int MAX_48_8B = 48 * 8;
    private final static int MAX_49_8B = 49 * 8;
    private final static int MAX_50_8B = 50 * 8;
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
            for (int j = 0; j < (PAGE_SIZE * 5 / (i + OBJ_HEADSIZE) + 10); j++) {
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
        int countSize46 = alloc_test(MAX_46_8B);
        int countSize47 = alloc_test(MAX_47_8B);
        int countSize48 = alloc_test(MAX_48_8B);
        int countSize49 = alloc_test(MAX_49_8B);
        int countSize50 = alloc_test(MAX_50_8B);
        //System.out.println(countSize46);
        //System.out.println(countSize47);
        //System.out.println(countSize48);
        //System.out.println(countSize49);
        //System.out.println(countSize50);

        if (countSize46 == 525 && countSize47 == 516 && countSize48 == 507 && countSize49 == 498 && countSize50 == 489)
            System.out.println("ExpectResult");
        else
            System.out.println("Error");
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n