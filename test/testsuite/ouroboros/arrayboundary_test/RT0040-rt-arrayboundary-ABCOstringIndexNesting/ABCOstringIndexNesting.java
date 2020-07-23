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
 * -@TestCaseID: Maple_ArrayBoundary_ABCOstringIndexNesting.java
 * -@TestCaseName: give nesting fun to String index
 * -@TestCaseType: Function Test
 * -@RequirementName: Array Bounds Check优化
 * -@Brief:
 * -#step1: new char[1024], change char[] to String
 * -#step2: for visit index, nestint func, funx, funy，funw to give value to index
 * -#step3: let char c = joinLine.charAt(index), when index > a.length, will catch ArrayIndexOutOfBoundsException
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ABCOstringIndexNesting.java
 * -@ExecuteClass: ABCOstringIndexNesting
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.util.Arrays;

public class ABCOstringIndexNesting {
    static int RES_PROCESS = 99;
    static int arrNum = 1023;

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
        char[] a = new char[1024];
        for (int i = 0; i < a.length; i++) {
            a[i] = 'w';
        }
        String joinLine = Arrays.toString(a);

        int index = 0;
        try {
            for (int j = 1; j <= joinLine.length(); j++) {
                if (j == joinLine.length()) {
                    index = func(j, index);
                    char c = joinLine.charAt(index);
                }
            }
        } catch (StringIndexOutOfBoundsException e) {
            res = 1;
        }
        return res;
    }

    public static int func(int index, int getIndex) {
        getIndex = funx(getIndex, index);
        getIndex = getIndex + 10;
        if (getIndex < index) {
            getIndex = func(index, getIndex);
        }
        return getIndex;
    }

    public static int funx(int getIndex, int index) {
        getIndex = getIndex + index;
        getIndex = funy(getIndex, index);
        return getIndex;
    }

    public static int funy(int getIndex, int index) {
        getIndex = getIndex * index;
        getIndex = funz(getIndex, index);
        return getIndex;
    }

    public static int funz(int getIndex, int index) {
        getIndex = getIndex / index;
        getIndex = funw(getIndex, index);
        return getIndex;
    }

    public static int funw(int getIndex, int index) {
        getIndex = getIndex - index;
        return getIndex;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n