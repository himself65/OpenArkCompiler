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
 * -@TestCaseID: Maple_ArrayBoundary_ABCOindexFor.java
 * -@TestCaseName: for visit Array[index], index visit 0 to Array.length.
 * -@TestCaseType: Function Test
 * -@RequirementName: Array Bounds Check优化
 * -@Brief:
 * -#step1: new Array[5]
 * -#step2: for visit Array element
 * -#step3: catch Exception
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ABCOindexFor.java
 * -@ExecuteClass: ABCOindexFor
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class ABCOindexFor {
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
        int res = 3 /*STATUS_FAILED*/;
        int[] a = new int[1000];
        int[] c = a;
        for (int ii = 0; ii < a.length; ii++) {
            c[ii] = ii;
        }

        int[] b = new int[a.length];
        try {
            for (int i = funx(100); i <= funx(1000); i++) {
                if (i >= 1000) {
                    for (int j = i; j > 0; j--) {
                        int index = funx(1000);
                        b[index] = a[index];
                    }
                }
            }
        } catch (ArrayIndexOutOfBoundsException e) {
            res = 1;
        }

        return res;
    }

    public static int funx(int maxFlag) {
        int endIndex = maxFlag + 5;
        int index;

        for (index = 0; index <= endIndex; index++) {
            index = funy(index);
        }

        return index;
    }

    public static int funy(int forIdx) {
        int idx = forIdx + 100;
        return idx;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n