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
 * -@TestCaseID: Maple_ArrayBoundary_ABCOemptyString.java
 * -@TestCaseName: String is "", index is 0
 * -@TestCaseType: Function Test
 * -@RequirementName: Array Bounds Check优化
 * -@Brief:
 * -#step1: String is "" or new String("")
 * -#step2: index is 0, call charAt and codePointAt
 * -#step3: catch Exception StringIndexOutOfBoundsException
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ABCOemptyString.java
 * -@ExecuteClass: ABCOemptyString
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class ABCOemptyString {
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

        try {
            String string = "";
            char c = string.charAt(0);
        } catch (StringIndexOutOfBoundsException e) {
            res--;
        }

        try {
            String string = new String("");
            int c = string.codePointAt(0);
        } catch (StringIndexOutOfBoundsException e) {
            res--;
        }
        return res;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n