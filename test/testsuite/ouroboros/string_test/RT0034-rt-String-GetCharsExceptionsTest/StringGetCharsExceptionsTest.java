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
 * -@TestCaseID: StringGetCharsExceptionsTest.java
 * -@TestCaseName: Test Exception in String Method: void getChars(int srcBegin, int srcEnd, char[] dst,
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      int dstBegin).
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create Parameters: dst is null, 0 < srcBegin < srcEnd, srcEnd < instance.length, dstBegin > 0.
 * -#step3: Test method getChars(int srcBegin, int srcEnd, char[] dst, int dstBegin), check NullPointerException is
 *          thrown, check dst is not replaced.
 * -#step4: Create Parameters: dst.length > 0, data.element is letter, srcBegin < 0, 0 < srcEnd < instance.length,
 *          0 < dstBegin < dst.length.
 * -#step5: Test method getChars(int srcBegin, int srcEnd, char[] dst, int dstBegin), check
 *          StringIndexOutOfBoundsException is thrown, check dst is not replaced.
 * -#step6: Create Parameters: dst.length > 0, data.element is letter, 0 < srcBegin < srcEnd, srcEnd < instance.length,
 *          0 < dstBegin < dst.length, dstBegin + srcEnd - srcBegin > dst.length.
 * -#step7: Test method getChars(int srcBegin, int srcEnd, char[] dst, int dstBegin), check
 *          ArrayIndexOutOfBoundsException is thrown, check dst is not replaced.
 * -#step8: Create Parameters: dst.length > 0, data.element is letter, srcBegin > srcEnd, 0 < srcEnd < instance.length,
 *          0 < dstBegin < dst.length.
 * -#step9: Test method getChars(int srcBegin, int srcEnd, char[] dst, int dstBegin), check
 *          StringIndexOutOfBoundsException is thrown, check dst is not replaced.
 * -#step10: Create Parameters: dst.length > 0, data.element is letter, 0 < srcBegin < srcEnd, srcEnd < instance.length,
 *           dstBegin < 0.
 * -#step11: Test method getChars(int srcBegin, int srcEnd, char[] dst, int dstBegin), check
 *           StringIndexOutOfBoundsException is thrown, check dst is not replaced.
 * -#step12: Change instance as One or more kinds of letters, numbers, special symbols/""/NoParam to repeat step2~11.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringGetCharsExceptionsTest.java
 * -@ExecuteClass: StringGetCharsExceptionsTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringGetCharsExceptionsTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringGetCharsExceptionsTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringGetCharsExceptionsTest_1() {
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
        char dst1_1[] = null;
        try {
            str.getChars(2, 5, dst1_1, 2);
        } catch (NullPointerException e1) {
            System.out.println("EXCEPTION 1_1");
        } catch (StringIndexOutOfBoundsException e1) {
            System.out.println("EXCEPTION 1_2");
        } finally {
            try {
                System.out.println(dst1_1);
            } catch (NullPointerException e2) {
                System.out.println("EXCEPTION 1_3");
            }
        }

        char dst1_2[] = {'A', 'B', 'C', 'D', 'E'};
        try {
            str.getChars(-1, 5, dst1_2, 2);
        } catch (StringIndexOutOfBoundsException e1) {
            System.out.println("EXCEPTION 2_1");
        } finally {
            try {
                System.out.println(dst1_2);
            } catch (NullPointerException e2) {
                System.out.println("EXCEPTION 2_2");
            }
        }

        char dst1_3[] = {'A', 'B', 'C', 'D', 'E'};
        try {
            str.getChars(2, 6, dst1_3, 2);
        } catch (ArrayIndexOutOfBoundsException e1) {
            System.out.println("EXCEPTION 3_1");
        } catch (StringIndexOutOfBoundsException e1) {
            System.out.println("EXCEPTION 3_2");
        } finally {
            try {
                System.out.println(dst1_3);
            } catch (NullPointerException e2) {
                System.out.println("EXCEPTION 3_3");
            }
        }

        char dst1_4[] = {'A', 'B', 'C', 'D', 'E'};
        try {
            str.getChars(4, 3, dst1_4, 2);
        } catch (StringIndexOutOfBoundsException e1) {
            System.out.println("EXCEPTION 4_1");
        } finally {
            try {
                System.out.println(dst1_4);
            } catch (NullPointerException e2) {
                System.out.println("EXCEPTION 4_2");
            }
        }

        char dst1_5[] = {'A', 'B', 'C', 'D', 'E'};
        try {
            str.getChars(4, 3, dst1_5, -1);
        } catch (StringIndexOutOfBoundsException e1) {
            System.out.println("EXCEPTION 5_1");
        } finally {
            try {
                System.out.println(dst1_5);
            } catch (NullPointerException e2) {
                System.out.println("EXCEPTION 5_2");
            }
        }
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full EXCEPTION 1_1\nEXCEPTION 1_3\nEXCEPTION 2_1\nABCDE\nEXCEPTION 3_1\nABCDE\nEXCEPTION 4_1\nABCDE\nEXCEPTION 5_1\nABCDE\nEXCEPTION 1_1\nEXCEPTION 1_3\nEXCEPTION 2_1\nABCDE\nEXCEPTION 3_1\nABCDE\nEXCEPTION 4_1\nABCDE\nEXCEPTION 5_1\nABCDE\nEXCEPTION 1_1\nEXCEPTION 1_3\nEXCEPTION 2_1\nABCDE\nEXCEPTION 3_1\nABCDE\nEXCEPTION 4_1\nABCDE\nEXCEPTION 5_1\nABCDE\nEXCEPTION 1_1\nEXCEPTION 1_3\nEXCEPTION 2_1\nABCDE\nEXCEPTION 3_2\nABCDE\nEXCEPTION 4_1\nABCDE\nEXCEPTION 5_1\nABCDE\nEXCEPTION 1_1\nEXCEPTION 1_3\nEXCEPTION 2_1\nABCDE\nEXCEPTION 3_2\nABCDE\nEXCEPTION 4_1\nABCDE\nEXCEPTION 5_1\nABCDE\nEXCEPTION 1_1\nEXCEPTION 1_3\nEXCEPTION 2_1\nABCDE\nEXCEPTION 3_1\nABCDE\nEXCEPTION 4_1\nABCDE\nEXCEPTION 5_1\nABCDE\nEXCEPTION 1_1\nEXCEPTION 1_3\nEXCEPTION 2_1\nABCDE\nEXCEPTION 3_1\nABCDE\nEXCEPTION 4_1\nABCDE\nEXCEPTION 5_1\nABCDE\nEXCEPTION 1_1\nEXCEPTION 1_3\nEXCEPTION 2_1\nABCDE\nEXCEPTION 3_1\nABCDE\nEXCEPTION 4_1\nABCDE\nEXCEPTION 5_1\nABCDE\n0\n