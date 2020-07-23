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
 * -@TestCaseID: StringConsBytesIntIntCharsetExceptionTest.java
 * -@TestCaseName: Test IndexOutOfBoundsException in String constructor: String(byte[] bytes, int offset, int
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 *                      length, Charset charset).
 * -@Brief:
 * -#step1: Create Parameters: byte[] str1_1,str1_1.length > 0, element is byte in normal.
 * -#step2: Create offset < 0, 0 < length < str1_1.length, Charset is a Unicode charset, Test Constructors new String(
 *          byte[] bytes, int offset, int length, Charset charset), check IndexOutOfBoundsException is thrown.
 * -#step3: Create str1_1.length > offset >= 0, length < 0, Charset is a Unicode Charset, Test Constructors new String(
 *          byte[] bytes, int offset, int length, Charset charset), check IndexOutOfBoundsException is thrown.
 * -#step4: Create str1_1.length > offset >= 0, length > str1_1.length, Charset is a Unicode Charset, Test Constructors
 *          new String(byte[] bytes, int offset, int length, Charset charset), check IndexOutOfBoundsException is thrown.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringConsBytesIntIntCharsetExceptionTest.java
 * -@ExecuteClass: StringConsBytesIntIntCharsetExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.nio.charset.Charset;

public class StringConsBytesIntIntCharsetExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv,  PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            result = StringConsBytesIntIntCharsetExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 96) {
            result = 0;
        }
        return result;
    }

    public static int StringConsBytesIntIntCharsetExceptionTest_1() {
        int result1 = 2; /* STATUS_Success */
        // IndexOutOfBoundsException - If the offset and length arguments index characters outside the bounds of the
        // bytes array
        byte[] str1_1 = new byte[]{(byte) 0x61, (byte) 0x62, (byte) 0x63, (byte) 0x31, (byte) 0x32, (byte) 0x33};
        try {
            String str1 = new String(str1_1, -1, 3, Charset.forName("ASCII"));
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        try {
            String str1 = new String(str1_1, 3, -1, Charset.forName("ASCII"));
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        try {
            String str1 = new String(str1_1, 3, 10, Charset.forName("ASCII"));
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        return result1;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n