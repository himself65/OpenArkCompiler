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
 * -@TestCaseID: Maple_ArrayBoundary_ABCOincreaseArray.java
 * -@TestCaseName: Array is increase length
 * -@TestCaseType: Function Test
 * -@RequirementName: Array Bounds Check优化
 * -@Brief:
 * -#step1: new Array[10] a, use System.arraycopy to create new Array[20] newArray
 * -#step2: visit a[9], no exception throw
 * -#step3: visit newArray[20], catch Exception
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ABCOincreaseArray.java
 * -@ExecuteClass: ABCOincreaseArray
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.lang.reflect.Array;
import java.util.Arrays;

public class ABCOincreaseArray {
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
        int res = 2 /*STATUS_FAILED*/;
        int[] a = new int[10];
        Arrays.fill(a, 20);
        int[] newArray = (int[]) func(a);
        int index = newArray[0];

        try {
            int c = a[index - 11];
        } catch (ArrayIndexOutOfBoundsException e) {
            res = 5;
        }

        try {
            int c = newArray[index];
        } catch (ArrayIndexOutOfBoundsException e) {
            res--;
        }

        return res;
    }

    public static Object func(Object array) {
        Class<?> clazz = array.getClass();
        if (clazz.isArray()) {
            Class<?> componentType = clazz.getComponentType();
            int length = Array.getLength(array);
            Object newArray = Array.newInstance(componentType, length + 10);
            System.arraycopy(array, 0, newArray, 0, length);
            return newArray;
        }
        return null;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n