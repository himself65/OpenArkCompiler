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
 * -@TestCaseID: Maple_ArrayBoundary_ABCOparam.java
 * -@TestCaseName: Array is get by function param
 * -@TestCaseType: Function Test
 * -@RequirementName: Array Bounds Check优化
 * -@Brief:
 * -#step1: new Array[5], Array is get by function param
 * -#step2: if visit Array
 * -#step3: catch Exception
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ABCOparam.java
 * -@ExecuteClass: ABCOparam
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class ABCOparam {
    static int RES_PROCESS = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out)); //
    }

    public static int run(String argv[], PrintStream out) {
        int result = 4 /*STATUS_FAILED*/;
        try {
            int[] arr = new int[5];
            result = test1(arr);
        } catch (Exception e) {
            RES_PROCESS -= 10;
        }

        if (result == 1 && RES_PROCESS == 99) {
            result = 0;
        }
        return result;
    }

    public static int test1(int[] arr) {
        int res = 3 /*STATUS_FAILED*/;

        int y = arr.length;
        try {
            if (y >= 0) {
                int c = arr[y];
            }
        } catch (ArrayIndexOutOfBoundsException e) {
            res = 1;
        }

        return res;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n