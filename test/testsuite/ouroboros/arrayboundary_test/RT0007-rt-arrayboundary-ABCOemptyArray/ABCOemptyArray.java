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
 * -@TestCaseID: Maple_ArrayBoundary_ABCOemptyArray.java
 * -@TestCaseName: new Array[0]
 * -@TestCaseType: Function Test
 * -@RequirementName: Array Bounds Check优化
 * -@Brief:
 * -#step1: new Array[0]
 * -#step2: visit index of const, index is 0 and -1
 * -#step3: catch Exception
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ABCOemptyArray.java
 * -@ExecuteClass: ABCOemptyArray
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class ABCOemptyArray {
    static int RES_PROCESS = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String argv[], PrintStream out) {
        int result = 4 /*STATUS_FAILED*/;
        int result2 = 4;
        try {
            result = test1();
            result2 = test2();
        } catch (Exception e) {
            RES_PROCESS -= 10;
        }

        if (result == 1 && result2 == 1 && RES_PROCESS == 99) {
            result = 0;
        }
        return result;
    }

    public static int test1() {
        int res = 3 /*STATUS_FAILED*/;
        Object[] a = new Object[0];
        try {
            a[0] = new Object[0];
        } catch (ArrayIndexOutOfBoundsException e) {
            res--;
        }

        try {
            Object c = a[-1];
        } catch (ArrayIndexOutOfBoundsException e) {
            res--;
        }
        return res;
    }

    public static int test2() {
        int res = 2 /*STATUS_FAILED*/;
        String[] data;
        try {
            data = new String[] {}; // 初始化
            data[0] = "lisi";
        } catch (ArrayIndexOutOfBoundsException e) {
            res--;
        }
        return res;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n