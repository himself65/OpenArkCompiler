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
 * -@TestCaseID: Maple_ArrayBoundary_ABCOStringMaxValue.java
 * -@TestCaseName: index is MAX_VALUE or MIN_VALUE
 * -@TestCaseType: Function Test
 * -@RequirementName: Array Bounds Check优化
 * -@Brief:
 * -#step1: new char[1024], change char[] to String.
 * -#step2: visit index of MAX_VALUE or MIN_VALUE const, use codePointCount
 * -#step3: catch Exception IndexOutOfBoundsException
 * -#step4: visit index of MAX_VALUE or MIN_VALUE const, use charAt
 * -#step5: catch Exception StringIndexOutOfBoundsException
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ABCOStringMaxValue.java
 * -@ExecuteClass: ABCOStringMaxValue
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.util.Arrays;

public class ABCOStringMaxValue {
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
        int res = 5 /*STATUS_FAILED*/;

        char[] a = new char[1024];
        for (int i = 0; i < a.length; i++) {
            a[i] = 'h';
        }
        String joinLine = Arrays.toString(a);

        int max = Integer.MAX_VALUE;
        int min = Integer.MIN_VALUE;
        try {
            int c = joinLine.codePointCount(min, 2);
        } catch (IndexOutOfBoundsException e) {
            res--;
        }

        try {
            char c = joinLine.charAt(min);
        } catch (StringIndexOutOfBoundsException e) {
            res--;
        }

        try {
            int c = joinLine.codePointCount(0, max);
        } catch (IndexOutOfBoundsException e) {
            res--;
        }

        try {
            char c = joinLine.charAt(max);
        } catch (StringIndexOutOfBoundsException e) {
            res--;
        }

        return res;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n