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
 * -@TestCaseID:Alloc_B256x8B
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title:ROS Allocator is in charge of applying and releasing objects.This testcase is mainly for testing big objects more than 2KB
 *- @Brief:functionTest
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: Alloc_B256x8B.java
 *- @ExecuteClass: Alloc_B256x8B
 *- @ExecuteArgs:
 */

import java.util.ArrayList;

public class Alloc_B256x8B {
    private final static int PAGE_SIZE = 4 * 1024;
    private final static int OBJ_HEADSIZE = 8;
    private final static int MAX_B256_8B = 2 * 1024 * 1024;
    private static ArrayList<byte[]> store;

    private static int alloc_test(int slot_type) {
        int sum = 0;
        store = new ArrayList<byte[]>();
        byte[] temp;

        for (int i = 1024 * 1024 - OBJ_HEADSIZE; i <= slot_type - OBJ_HEADSIZE; i = i + 256) {
            for (int j = 0; j < (PAGE_SIZE * 2560 / (i + OBJ_HEADSIZE) + 5); j++) {
                temp = new byte[i];
                store.add(temp);
            }
            sum += store.size();
            store = new ArrayList<byte[]>();
        }
        return sum;
    }

    public static void main(String[] args) {
        store = new ArrayList<byte[]>();
        int result = alloc_test(MAX_B256_8B);
        //System.out.println(result);
        if (result == 46939)
            System.out.println("ExpectResult");
        else
            System.out.println("Error");
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n