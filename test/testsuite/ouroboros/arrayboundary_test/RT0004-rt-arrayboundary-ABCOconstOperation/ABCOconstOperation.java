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
 * -@TestCaseID: Maple_ArrayBoundary_ABCOconstOperation.java
 * -@TestCaseName: index is X,  X = Y +/-/X///&/|/ Const
 * -@TestCaseType: Function Test
 * -@RequirementName: Array Bounds Check优化
 * -@Brief:
 * -#step1: new Array[5]
 * -#step2: visit index is X,  X = Y +/-/X///&/|/ Const, X is large than arr.length
 * -#step3: catch Exception
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ABCOconstOperation.java
 * -@ExecuteClass: ABCOconstOperation
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class ABCOconstOperation {
    static int RES_PROCESS = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String argv[], PrintStream out) {
        int result = 4 /*STATUS_FAILED*/;
        try {
            result = test1();
        } catch (Exception e) {
            RES_PROCESS -= 10;
        }

        if (result == 1 && RES_PROCESS == 99) {
            result = 0;
        }
        return result;
    }

    public static int test1() {
        int res = 10 /*STATUS_FAILED*/;
        int[] arr1 = new int[15];
        for (int i = 0; i < arr1.length; i++) {
            arr1[i] = i;
        }
        int index = 1;

        int y = 6;
        try {
            index = index + y + 9;
            int c = arr1[index];
        } catch (ArrayIndexOutOfBoundsException e) {
            res--;
        }

        try {
            index = y - 16 - index;
            int c = arr1[index];
        } catch (ArrayIndexOutOfBoundsException e) {
            res--;
        }

        try {
            index = y * y * index;
            int c = arr1[index];
        } catch (ArrayIndexOutOfBoundsException e) {
            res--;
        }

        try {
            index = index + 937;
            index = (y / index) * y;
            int c = arr1[index];
        } catch (ArrayIndexOutOfBoundsException e) {
            res--;
        }

        try {
            index = (y + 9) & 15;
            int c = arr1[index];
        } catch (ArrayIndexOutOfBoundsException e) {
            res--;
        }

        try {
            index = y | index;
            int c = arr1[index];
        } catch (ArrayIndexOutOfBoundsException e) {
            res--;
        }

        try {
            index = ~index;
            int c = arr1[index];
        } catch (ArrayIndexOutOfBoundsException e) {
            res--;
        }

        try {
            index = 15;
            index = index ^ 16;
            int c = arr1[index];
        } catch (ArrayIndexOutOfBoundsException e) {
            res--;
        }

        try {
            index = index % 16;
            int c = arr1[index];
        } catch (ArrayIndexOutOfBoundsException e) {
            res--;
        }
        return res;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n