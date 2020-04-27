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
 * -@TestCaseID: StringNullPointExceptionTest.java
 * -@TestCaseName: Test String Method: Test NullPointException throw in String Method: public char charAt (int
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      index), public char[] toCharArray (), public String substring (int beginIndex), public int
 *                      compareTo (String anotherString), public String intern (), public String replace (CharSequence
 *                      target, CharSequence replacement), public String concat (String str).
 * -@Brief:
 * -#case1
 * -#step1: Create String instance, parameter index < 0.
 * -#step2: Test method charAt (int index), check StringIndexOutOfBoundsException is thrown.
 * -#case2
 * -#step1: Create null String instance, parameter index >= 0.
 * -#step2: Test method charAt (int index), check NullPointerException is thrown.
 * -#case3
 * -#step1: Create null String instance.
 * -#step2: Test method toCharArray(), check NullPointerException is thrown.
 * -#case4
 * -#step1: Create null String instance, parameter beginIndex >= 0.
 * -#step2: Test method substring (int beginIndex), check NullPointerException is thrown.
 * -#case5
 * -#step1: Create null String instance, parameter anotherString as string instance.
 * -#step2: Test method compareTo (String anotherString), check NullPointerException is thrown.
 * -#case6
 * -#step1: Create null String instance.
 * -#step2: Test method intern (), check NullPointerException is thrown.
 * -#case7
 * -#step1: Create null String instance, parameter target/replacement as CharSequence instance.
 * -#step2: Test method replace (CharSequence target, CharSequence replacement), check NullPointerException is thrown.
 * -#case8
 * -#step1: Create null String instance, parameter str as String instance.
 * -#step2: Test method concat (String str), check NullPointerException is thrown.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: StringNullPointExceptionTest.java
 * -@ExecuteClass: StringNullPointExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringNullPointExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2;  /* STATUS_Success */
        try {
            StringNullPointExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 83) {
            result = 0;
        }
        return result;
    }

    public static void StringNullPointExceptionTest_1() {
        String str2_1 = "abc123";
        String str2_5 = null;

        test0(str2_1);
        test1(str2_5);
        test2(str2_5);
        test3(str2_5);
        test4(str2_5);
        test5(str2_5);
        test6(str2_5);
        test7(str2_5);
    }

    private static void test0(String str) {
        try {
            char t0 = str.charAt(-2);
        } catch (StringIndexOutOfBoundsException e) {
            processResult -= 2;
        }
    }

    private static void test1(String str) {
        try {
            char t1 = str.charAt(0);
        } catch (NullPointerException e) {
            processResult -= 2;
        }
    }

    private static void test2(String str) {
        char[] t2 = null;
        try {
            t2 = str.toCharArray();
        } catch (NullPointerException e) {
            processResult -= 2;
        }
    }

    private static void test3(String str) {
        String t3 = null;
        try {
            t3 = str.substring(1);
        } catch (NullPointerException e) {
            processResult -= 2;
        }
    }

    private static void test4(String str) {
        try {
            int t4 = str.compareTo("aaa");
        } catch (NullPointerException e) {
            processResult -= 2;
        }
    }

    private static void test5(String str) {
        try {
            String t5 = str.intern();
        } catch (NullPointerException e) {
            processResult -= 2;
        }
    }

    private static void test6(String str) {
        try {
            String t6 = str.replace("a", "b");
        } catch (NullPointerException e) {
            processResult -= 2;
        }
    }

    private static void test7(String str) {
        try {
            String t7 = str.concat("aaa");
        } catch (NullPointerException e) {
            processResult -= 2;
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n