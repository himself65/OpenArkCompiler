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
 * -@TestCaseID: StringGetCharsTest.java
 * -@TestCaseName: Test String Method: void getChars(int srcBegin, int srcEnd, char[] dst, int dstBegin).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create parameters: dst.length > 0, dst.element some letters. 0 =< srcBegin < srcEnd,
 *          srcEnd < instance.length. 0 =< dstBegin < dst.length -1, srcBegin/srcEnd/dstBegin cover boundary value.
 * -#step3: Test method getChars(int srcBegin, int srcEnd, char[] dst, int dstBegin).
 * -#step4: check the dst is replaced correctly.
 * -#step5: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~4.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringGetCharsTest.java
 * -@ExecuteClass: StringGetCharsTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringGetCharsTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv,  PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringGetCharsTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringGetCharsTest_1() {
        String str1_1 = new String("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        String str1_2 = new String(" @!.&%");
        String str1_3 = new String("abc123");

        String str2_1 = "qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96";
        String str2_2 = " @!.&%";
        String str2_3 = "abc123";

        test(str1_1);
        test(str1_2);
        test(str1_3);
        test(str2_1);
        test(str2_2);
        test(str2_3);
    }

    private static void test(String str) {
        // 0 < srcBegin < srcEnd, srcEnd < instance.length. 0 =< dstBegin < dst.length -1.
        char[] dst = {'A', 'B', 'C', 'D', 'E', 'F'};
        str.getChars(2, 5, dst, 2);
        System.out.println(dst);

        // srcBegin = 0, srcEnd < instance.length. 0 =< dstBegin < dst.length -1.
        dst = new char[]{'A', 'B', 'C', 'D', 'E', 'F'};
        str.getChars(0, 3, dst, 2);
        System.out.println(dst);

        // srcBegin = instance.length, srcEnd = instance.length, 0 =< dstBegin < dst.length -1.
        dst = new char[]{'A', 'B', 'C', 'D', 'E', 'F'};
        str.getChars(str.length(), str.length(), dst, 2);
        System.out.println(dst);

        // srcBegin = 0, srcEnd = 0, 0 =< dstBegin < dst.length -1.
        dst = new char[]{'A', 'B', 'C', 'D', 'E', 'F'};
        str.getChars(0, 0, dst, 2);
        System.out.println(dst);

        // 0 < srcBegin < srcEnd, srcEnd < instance.length, dstBegin = 0.
        dst = new char[]{'A', 'B', 'C', 'D', 'E', 'F'};
        str.getChars(2, 3, dst, 0);
        System.out.println(dst);

        // 0 < srcBegin < srcEnd, srcEnd < instance.length, dstBegin = dst.length -1.
        dst = new char[]{'A', 'B', 'C', 'D', 'E', 'F'};
        str.getChars(2, 3, dst, 5);
        System.out.println(dst);
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ABertF\nABqweF\nABCDEF\nABCDEF\neBCDEF\nABCDEe\nAB!.&F\nAB @!F\nABCDEF\nABCDEF\n!BCDEF\nABCDE!\nABc12F\nABabcF\nABCDEF\nABCDEF\ncBCDEF\nABCDEc\nABertF\nABqweF\nABCDEF\nABCDEF\neBCDEF\nABCDEe\nAB!.&F\nAB @!F\nABCDEF\nABCDEF\n!BCDEF\nABCDE!\nABc12F\nABabcF\nABCDEF\nABCDEF\ncBCDEF\nABCDEc\n0\n
