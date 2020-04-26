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
 * -@TestCaseID: Maple_ArrayBoundary_ABCOStringConstOperation.java
 * -@TestCaseName: index is X,  X = Y +/-/X///&/|/ Const
 * -@TestCaseType: Function Test
 * -@RequirementName: Array Bounds Check优化
 * -@Brief:
 * -#step1: new char[1024], change char[] to String.
 * -#step2: visit index is X,  X = Y +/-/X///&/|/ Const, X is large than length
 * -#step3: catch Exception
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ABCOStringConstOperation.java
 * -@ExecuteClass: ABCOStringConstOperation
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.util.Arrays;

public class ABCOStringConstOperation {
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
        byte[] a = new byte[1024];
        for (int i = 0; i < a.length; i++) {
            a[i] = '1';
        }
        String joinLine = Arrays.toString(a);

        int index = 1;
        int y = joinLine.length();
        try {
            index = index + y + 9;
            String c = joinLine.substring(index);
        } catch (StringIndexOutOfBoundsException e) {
            res--;
        }

        try {
            index = y - joinLine.length() - index;
            String c = joinLine.substring(index);
        } catch (StringIndexOutOfBoundsException e) {
            res--;
        }

        try {
            index = y * y * index;
            String c = joinLine.substring(index);
        } catch (StringIndexOutOfBoundsException e) {
            res--;
        }

        try {
            index = index + 167772161;
            index = (y / index) * y;
            String c = joinLine.substring(index);
        } catch (StringIndexOutOfBoundsException e) {
            res--;
        }

        try {
            index = index & index;
            String c = joinLine.substring(index);
        } catch (StringIndexOutOfBoundsException e) {
            res--;
        }

        try {
            index = y | index;
            String c = joinLine.substring(index);
        } catch (StringIndexOutOfBoundsException e) {
            res--;
        }

        try {
            index = ~index;
            String c = joinLine.substring(index);
        } catch (StringIndexOutOfBoundsException e) {
            res--;
        }

        try {
            index = y + y;
            index = index ^ y;
            String c = joinLine.substring(index);
        } catch (StringIndexOutOfBoundsException e) {
            res--;
        }

        try {
            index = index + index;
            y = 8195;
            index = index % y;
            String c = joinLine.substring(index);
        } catch (StringIndexOutOfBoundsException e) {
            res--;
        }
        return res;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n