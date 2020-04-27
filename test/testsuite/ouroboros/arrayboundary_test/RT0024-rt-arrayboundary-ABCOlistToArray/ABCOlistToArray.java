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
 * -@TestCaseID: Maple_ArrayBoundary_ABCOlistToArray.java
 * -@TestCaseName: index from Integer.parseInt, Array from list.toArray
 * -@TestCaseType: Function Test
 * -@RequirementName: Array Bounds Check优化
 * -@Brief:
 * -#step1: new Array, Array from list.toArray
 * -#step2: index from Integer.parseInt
 * -#step3: catch Exception
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ABCOlistToArray.java
 * -@ExecuteClass: ABCOlistToArray
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.util.ArrayList;
import java.util.List;

public class ABCOlistToArray {
    static int RES_PROCESS = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out)); //
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
        int res = 3 /*STATUS_FAILED*/;
        String[] a = func();
        int x = -1;
        try {
            String c = a[x];
        } catch (ArrayIndexOutOfBoundsException e) {
            res--;
        }
        x = Integer.parseInt(a[a.length - 1]);
        try {
            String c = a[x];
        } catch (ArrayIndexOutOfBoundsException e) {
            res--;
        }
        return res;
    }

    public static String[] func() {
        List<String> list = new ArrayList<>();
        list.add("1");
        list.add("2");
        list.add("3");
        list.add("4");
        list.add("5");
        String[] arr = new String[list.size()];
        try {
            list.toArray(arr);
        } catch (Exception e) {
            e.printStackTrace();
        }

        return arr;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n