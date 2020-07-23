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
 * -@TestCaseID: StringCreateStringArrayTest.java
 * -@TestCaseName: Create String Array and Test String Array as param of function.
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String array.
 * -#step2: Check that String array is created correctly.
 * -#step3: Create a string[] as param of function.
 * -#step4: Check the return value of function is correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringCreateStringArrayTest.java
 * -@ExecuteClass: StringCreateStringArrayTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringCreateStringArrayTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringCreateStringArrayTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringCreateStringArrayTest_1() {
        String[] strArray = {"a", "b", "c", "1", "2", "3"};
        System.out.println(strArray[1]);
        System.out.println(test(strArray)[1]);
    }

    private static String[] test(String[] str) {
        for (int i = 0; i < str.length; i++) {
            System.out.println(str[i]);
        }

        str[1] = "B";
        return str;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full b\na\nb\nc\n1\n2\n3\nB\n0\n