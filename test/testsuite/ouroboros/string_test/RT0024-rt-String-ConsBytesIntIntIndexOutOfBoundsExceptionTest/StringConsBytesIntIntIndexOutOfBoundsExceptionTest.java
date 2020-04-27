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
 * -@TestCaseID: StringConsBytesIntIntIndexOutOfBoundsExceptionTest.java
 * -@TestCaseName: Test IndexOutOfBoundsException in String constructor:public String(byte[] bytes, int offset,
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
  *                     int length).
 * -@Brief:
 * -#step1: Create Parameters: byte[] str1_1,str1_1.length > 0, element is byte in normal.
 * -#step2: Create offset < 0, 0 < length < str1_1.length, Test Constructors new String(byte[] bytes, int offset,
 *          int length), check IndexOutOfBoundsException is thrown.
 * -#step3: Create 0 < offset < str1_1.length, length < 0, Test Constructors new String(byte[] bytes, int offset,
 *          int length), check IndexOutOfBoundsException is thrown.
 * -#step4: Create 0 < offset < str1_1.length, length > str1_1.length, Test Constructors new String(byte[] bytes,
 *          int offset, int length), check IndexOutOfBoundsException is thrown.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringConsBytesIntIntIndexOutOfBoundsExceptionTest.java
 * -@ExecuteClass: StringConsBytesIntIntIndexOutOfBoundsExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringConsBytesIntIntIndexOutOfBoundsExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv,  PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            result = StringConsBytesIntIntIndexOutOfBoundsExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 96) {
            result = 0;
        }
        return result;
    }

    public static int StringConsBytesIntIntIndexOutOfBoundsExceptionTest_1() {
        int result1 = 2; /* STATUS_Success */
        // IndexOutOfBoundsException - If the offset and length arguments index characters outside the bounds of the
        // bytes array
        byte[] str1_1 = new byte[]{(byte) 0x61, (byte) 0x62, (byte) 0x63, (byte) 0x31, (byte) 0x32, (byte) 0x33};
        try {
            String str1 = new String(str1_1, -1, 3);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        try {
            String str1 = new String(str1_1, 3, -1);
            processResult -= 10;
        } catch (IndexOutOfBoundsException e1) {
            processResult--;
        }
        try {
            String str1 = new String(str1_1, 3, 10);
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