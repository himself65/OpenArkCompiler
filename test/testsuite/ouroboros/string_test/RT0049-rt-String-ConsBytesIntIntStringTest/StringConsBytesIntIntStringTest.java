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
 * -@TestCaseID: StringConsBytesIntIntStringTest.java
 * -@TestCaseName: Test String constructor: String(byte[] bytes, int offset, int length, String charsetName).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create Parameters: bytes.length > 0, bytes.element is byte number.
 * -#step2: Create Parameters: offset = 0, 0 < length < bytes.length, charsetName is Charset name.
 * -#step3: Test String(byte[] bytes, int offset, int length, String charsetName), Check the return String is correctly.
 * -#step4: Create Parameters: offset = 0, length = bytes.length, charsetName is Charset name.
 * -#step5: Test String(byte[] bytes, int offset, int length, String charsetName), Check the return String is correctly.
 * -#step6: Create Parameters: offset = 0, length = 0, charsetName is Charset name.
 * -#step7: Test String(byte[] bytes, int offset, int length, String charsetName), Check the return String is correctly.
 * -#step8: Create Parameters: offset = bytes.length - 1, length = 1, charsetName is Charset name.
 * -#step9: Test String(byte[] bytes, int offset, int length, String charsetName), Check the return String is correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringConsBytesIntIntStringTest.java
 * -@ExecuteClass: StringConsBytesIntIntStringTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.io.UnsupportedEncodingException;


public class StringConsBytesIntIntStringTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringConsBytesIntIntStringTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringConsBytesIntIntStringTest_1() throws UnsupportedEncodingException {
        byte[] str1 = new byte[]{(byte) 0x61, (byte) 0x62, (byte) 0x63, (byte) 0x31, (byte) 0x32, (byte) 0x33};
        String str1_1 = new String(str1, 0, 3, "ASCII");
        String str1_2 = new String(str1, 0, 6, "ASCII");
        String str1_3 = new String(str1, 0, 0, "ASCII");
        String str1_4 = new String(str1, 5, 1, "ASCII");
        System.out.println(str1_1);
        System.out.println(str1_2);
        System.out.println(str1_3);
        System.out.println(str1_4);
    }
}





// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full abc\nabc123\n\n3\n0\n