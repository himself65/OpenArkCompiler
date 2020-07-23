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
 * -@TestCaseID: StringGetBytesIntIntBytesIntTest.java
 * -@TestCaseName: Test String Method: void getBytes(int srcBegin, int srcEnd, byte[] dst, int dstBegin).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create parameters: dst.length > 0, dst.element is Hex number. 0 =< srcBegin < srcEnd, srcEnd <
 *          instance.length. 0 =< dstBegin < dst.length -1. srcBegin/srcEnd/dstBegin cover boundary value.
 * -#step3: Test method getBytes(int srcBegin, int srcEnd, byte[] dst, int dstBegin).
 * -#step4: check the dst is replaced correctly.
 * -#step5: Change instance as One or more kinds of letters, numbers, special symbols to repeat step2~4.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringGetBytesIntIntBytesIntTest.java
 * -@ExecuteClass: StringGetBytesIntIntBytesIntTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringGetBytesIntIntBytesIntTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringGetBytesIntIntBytesIntTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringGetBytesIntIntBytesIntTest_1() {
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
        // Test 0 < srcBegin < srcEnd, srcEnd < instance.length. 0 =< dstBegin < dst.length -1.
        byte[] dst = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
        str.getBytes(2, 5, dst, 2);
        String str_new = new String(dst);
        System.out.println(str_new);

        // Test srcBegin = 0, srcEnd < instance.length. 0 =< dstBegin < dst.length -1.
        dst = new byte[]{0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
        str.getBytes(0, 3, dst, 2);
        str_new = new String(dst);
        System.out.println(str_new);

        // Test srcBegin = instance.length, srcEnd = instance.length, 0 =< dstBegin < dst.length -1.
        dst = new byte[]{0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
        str.getBytes(str.length(), str.length(), dst, 2);
        str_new = new String(dst);
        System.out.println(str_new);

        // Test srcBegin = 0, srcEnd = 0, 0 =< dstBegin < dst.length -1.
        dst = new byte[]{0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
        str.getBytes(0, 0, dst, 2);
        str_new = new String(dst);
        System.out.println(str_new);

        // Test 0 < srcBegin < srcEnd, srcEnd < instance.length, dstBegin = 0.
        dst = new byte[]{0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
        str.getBytes(2, 3, dst, 0);
        str_new = new String(dst);
        System.out.println(str_new);

        // Test 0 < srcBegin < srcEnd, srcEnd < instance.length, dstBegin = dst.length -1.
        dst = new byte[]{0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
        str.getBytes(2, 3, dst, 5);
        str_new = new String(dst);
        System.out.println(str_new);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ABertF\nABqweF\nABCDEF\nABCDEF\neBCDEF\nABCDEe\nAB!.&F\nAB @!F\nABCDEF\nABCDEF\n!BCDEF\nABCDE!\nABc12F\nABabcF\nABCDEF\nABCDEF\ncBCDEF\nABCDEc\nABertF\nABqweF\nABCDEF\nABCDEF\neBCDEF\nABCDEe\nAB!.&F\nAB @!F\nABCDEF\nABCDEF\n!BCDEF\nABCDE!\nABc12F\nABabcF\nABCDEF\nABCDEF\ncBCDEF\nABCDEc\n0\n
