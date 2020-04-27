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
 * -@TestCaseID: StringGetBytesExceptionTest.java
 * -@TestCaseName: Test Exception in String Method: getBytes(int srcBegin, int srcEnd, byte[] dst, int dstBegin).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create Parameters: dst is null, 0 < srcBegin < srcEnd, srcEnd < instance.length, dstBegin > 0.
 * -#step3: Test method getBytes(int srcBegin, int srcEnd, byte[] dst, int dstBegin), check NullPointerException is
 *          thrown, check dst is not replaced.
 * -#step4: Create Parameters: dst.length > 0, data.element is normal byte, srcBegin < 0, 0 < srcEnd < instance.length,
 *          0 < dstBegin < dst.length.
 * -#step5: Test method getBytes(int srcBegin, int srcEnd, byte[] dst, int dstBegin), check
 *          StringIndexOutOfBoundsException is thrown, check dst is not replaced.
 * -#step6: Create Parameters: dst.length > 0, data.element is normal byte, 0 < srcBegin < srcEnd, srcEnd <
 *          instance.length, 0 < dstBegin < dst.length, dstBegin + srcEnd - srcBegin > dst.length.
 * -#step7: Test method getBytes(int srcBegin, int srcEnd, byte[] dst, int dstBegin), check
 *          ArrayIndexOutOfBoundsException is thrown, check dst is not replaced.
 * -#step8: Create Parameters: dst.length > 0, data.element is normal byte, srcBegin > srcEnd, 0 < srcEnd
 *          < instance.length, 0 < dstBegin < dst.length.
 * -#step9: Test method getBytes(int srcBegin, int srcEnd, byte[] dst, int dstBegin), check
 *          StringIndexOutOfBoundsException is thrown, check dst is not replaced.
 * -#step10: Create Parameters: dst.length > 0, data.element is normal byte, 0 < srcBegin < srcEnd, srcEnd <
 *           instance.length, dstBegin < 0.
 * -#step11: Test method getBytes(int srcBegin, int srcEnd, byte[] dst, int dstBegin), check
 *           StringIndexOutOfBoundsException is thrown, check dst is not replaced.
 * -#step12: Change instance as One or more kinds of letters, numbers, special symbols/""/NoParam to repeat step2~11.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringGetBytesExceptionTest.java
 * -@ExecuteClass: StringGetBytesExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringGetBytesExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringGetBytesExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringGetBytesExceptionTest_1() {
        String str1_1 = new String("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        String str1_2 = new String(" @!.&%");
        String str1_3 = new String("abc123");
        String str1_4 = new String("");
        String str1_5 = new String();

        String str2_1 = "qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890-=!" +
                "@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96";
        String str2_2 = " @!.&%";
        String str2_3 = "abc123";

        test(str1_1);
        test(str1_2);
        test(str1_3);
        test(str1_4);
        test(str1_5);
        test(str2_1);
        test(str2_2);
        test(str2_3);
    }

    private static void test(String str) {
        byte[] dst1_1 = null;
        try {
            str.getBytes(2, 5, dst1_1, 2);
            String str_new = new String(dst1_1);
        } catch (NullPointerException e1) {
            System.out.println("EXCEPTION 1_1");
        } catch (StringIndexOutOfBoundsException e1) {
            System.out.println("EXCEPTION 1_2");
        }

        byte[] dst1_2 = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
        try {
            str.getBytes(-1, 5, dst1_2, 2);
            String str_new = new String(dst1_2);
        } catch (StringIndexOutOfBoundsException e1) {
            System.out.println("EXCEPTION 2_1");
        }

        // if (srcEnd > length()) ...throw new StringIndexOutOfBoundsException(this, srcEnd);
        byte[] dst1_3 = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
        try {
            str.getBytes(2, 6, dst1_3, 2);
            String str_new = new String(dst1_3);
        } catch (ArrayIndexOutOfBoundsException e1) {
            System.out.println("EXCEPTION 3_1");
        } catch (StringIndexOutOfBoundsException e1) {
            System.out.println("EXCEPTION 3_2");
        }

        byte[] dst1_4 = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
        try {
            str.getBytes(4, 3, dst1_4, 2);
            String str_new = new String(dst1_4);
        } catch (StringIndexOutOfBoundsException e1) {
            System.out.println("EXCEPTION 4_1");
        }

        byte[] dst1_5 = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
        try {
            str.getBytes(4, 3, dst1_5, -1);
            String str_new = new String(dst1_5);
        } catch (StringIndexOutOfBoundsException e1) {
            System.out.println("EXCEPTION 5_1");
        }
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan EXCEPTION\s*1_1\s*EXCEPTION\s*2_1\s*EXCEPTION\s*4_1\s*EXCEPTION\s*5_1\s*EXCEPTION\s*1_1\s*EXCEPTION\s*2_1\s*EXCEPTION\s*4_1\s*EXCEPTION\s*5_1\s*EXCEPTION\s*1_1\s*EXCEPTION\s*2_1\s*EXCEPTION\s*4_1\s*EXCEPTION\s*5_1\s*EXCEPTION\s*1_2\s*EXCEPTION\s*2_1\s*EXCEPTION\s*3_2\s*EXCEPTION\s*4_1\s*EXCEPTION\s*5_1\s*EXCEPTION\s*1_2\s*EXCEPTION\s*2_1\s*EXCEPTION\s*3_2\s*EXCEPTION\s*4_1\s*EXCEPTION\s*5_1\s*EXCEPTION\s*1_1\s*EXCEPTION\s*2_1\s*EXCEPTION\s*4_1\s*EXCEPTION\s*5_1\s*EXCEPTION\s*1_1\s*EXCEPTION\s*2_1\s*EXCEPTION\s*4_1\s*EXCEPTION\s*5_1\s*EXCEPTION\s*1_1\s*EXCEPTION\s*2_1\s*EXCEPTION\s*4_1\s*EXCEPTION\s*5_1\s*0