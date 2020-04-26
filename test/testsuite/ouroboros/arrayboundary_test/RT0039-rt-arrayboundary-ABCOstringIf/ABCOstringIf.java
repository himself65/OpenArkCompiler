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
 * -@TestCaseID: Maple_ArrayBoundary_ABCOstringIf.java
 * -@TestCaseName: if visit String
 * -@TestCaseType: Function Test
 * -@RequirementName: Array Bounds Check优化
 * -@Brief:
 * -#step1: new String
 * -#step2: if visit String, index = String.length()
 * -#step3: catch Exception
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ABCOstringIf.java
 * -@ExecuteClass: ABCOstringIf
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.util.Arrays;
import java.util.Random;

public class ABCOstringIf {
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
        char[] a = new char[1024];
        for (int i = 0; i < a.length; i++) {
            a[i] = 'h';
        }
        String joinLine = Arrays.toString(a);

        int y = func();
        try {
            if (y >= 0) {
                char c = joinLine.charAt(y);
                ;
            }
        } catch (StringIndexOutOfBoundsException e) {
            res = 1;
        }

        return res;
    }

    public static int test2() {
        int res = 3 /*STATUS_FAILED*/;
        char[] a = new char[1024];
        for (int i = 0; i < a.length; i++) {
            a[i] = 'h';
        }
        String joinLine = Arrays.toString(a);

        try {
            for (int i = 0; i < joinLine.length(); i++) {
                if (joinLine.charAt(i) == joinLine.charAt(i + 1)) {
                    char c = joinLine.charAt(i);
                }
            }
        } catch (StringIndexOutOfBoundsException e) {
            res = 1;
        }

        return res;
    }

    public static int func() {
        Random r = new Random();
        int get = r.nextInt(6);
        if (get >= 5) {
            return get + 3072;
        } else {
            return get + 3072;
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n