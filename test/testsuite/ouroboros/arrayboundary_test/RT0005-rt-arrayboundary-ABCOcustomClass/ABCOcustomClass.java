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
 * -@TestCaseID: Maple_ArrayBoundary_ABCOcustomClass.java
 * -@TestCaseName: index and array is in same customClass, and another class call array[index].
 * -@TestCaseType: Function Test
 * -@RequirementName: Array Bounds Check优化
 * -@Brief:
 * -#step1: create customClass include index and arrays
 * -#step2: get customClass.arrays[index], index is arrays.length
 * -#step3: catch Exception
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ABCOcustomClass.java
 * -@ExecuteClass: ABCOcustomClass
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class ABCOcustomClass {
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
        People p = new People(3);
        int x = p.n;
        try {
            Student a = p.s[x];
        } catch (ArrayIndexOutOfBoundsException e) {
            res = 1;
        }
        return res;
    }
}

class People {
    int n;
    Student s[] = new Student[n];

    public People(int n) {
        this.n = n;
    }
}

class Student {
    int number;
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n