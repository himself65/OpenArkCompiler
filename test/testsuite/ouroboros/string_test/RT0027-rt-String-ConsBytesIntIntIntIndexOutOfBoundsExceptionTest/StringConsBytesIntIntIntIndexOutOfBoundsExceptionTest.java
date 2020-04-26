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
 * -@TestCaseID: StringConsBytesIntIntIntIndexOutOfBoundsExceptionTest.java
 * -@TestCaseName: Test IndexOutOfBoundsException/IllegalArgumentException in String constructor: String(byte[]
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      ascii, int hibyte, int offset, int count).
 * -@Brief:
 * -#step1: Create Parameters: byte[] str1_1,str1_1.length > 0, element is byte in normal.
 * -#step2: Create hibyte >= 0, offset < 0, 0 < count < str1_1.length, Test Constructors new String(byte[] bytes, int
 *          hibyte, int offset, int count), check IndexOutOfBoundsException is thrown.
 * -#step3: Create hibyte >= 0, str1_1.length > offset >= 0, count > str1_1.length, Test Constructors new String(byte[]
 *          bytes, int hibyte, int offset, int count), check IndexOutOfBoundsException is thrown.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringConsBytesIntIntIntIndexOutOfBoundsExceptionTest.java
 * -@ExecuteClass: StringConsBytesIntIntIntIndexOutOfBoundsExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringConsBytesIntIntIntIndexOutOfBoundsExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv,  PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            result = StringConsBytesIntIntIntIndexOutOfBoundsExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 97) {
            result = 0;
        }
        return result;
    }

    public static int StringConsBytesIntIntIntIndexOutOfBoundsExceptionTest_1() {
        int result1 = 2; /* STATUS_Success */
        // IndexOutOfBoundsException - If the offset and count arguments index characters outside the bounds of the
        // codePoints array
        byte[] str1_1 = new byte[]{(byte) 0x61, (byte) 0x62, (byte) 0x63, (byte) 0x31, (byte) 0x32, (byte) 0x33};
        try {
            String str1 = new String(str1_1, 0, -1, 2);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        try {
            String str1 = new String(str1_1, 0, 0, 10);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        return result1;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0