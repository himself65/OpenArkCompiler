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
 * -@TestCaseID: StringCosBytesIntIntCharsetTest.java
 * -@TestCaseName: String constructor: String(byte[] bytes, int offset, int length, Charset charset).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create Parameters: bytes.length > 0, bytes.element is byte number.
 * -#step2: Create Parameters: 0 =< offset < bytes.length, 0 =< length < bytes.length, charset like ASCII.charset.
 * -#step3: Test String(byte[] bytes, int offset, int length, Charset charset), check the return String is correctly.
 * -#step4: Create Parameters: offset = 0, length = bytes.length, charset like ASCII.charset.
 * -#step5: Test String(byte[] bytes, int offset, int length, Charset charset), Check the return String is correctly.
 * -#step6: Create Parameters: offset = 0, length = 0, charset like ASCII.charset.
 * -#step7: Test String(byte[] bytes, int offset, int length, Charset charset), Check the return String is correctly.
 * -#step8: Create Parameters: offset = bytes.length - 1, length = 1, charset like ASCII.charset.
 * -#step9: Test String(byte[] bytes, int offset, int length, Charset charset), Check the return String is correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringCosBytesIntIntCharsetTest.java
 * -@ExecuteClass: StringCosBytesIntIntCharsetTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.io.UnsupportedEncodingException;
import java.nio.charset.Charset;

public class StringCosBytesIntIntCharsetTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringCosBytesIntIntCharsetTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringCosBytesIntIntCharsetTest_1() throws UnsupportedEncodingException {
        byte[] str1 = new byte[]{(byte) 0x61, (byte) 0x62, (byte) 0x63, (byte) 0x31, (byte) 0x32, (byte) 0x33};
        String str1_1 = new String(str1, 3, 3, Charset.forName("ASCII"));
        String str1_2 = new String(str1, 0, 6, Charset.forName("ASCII"));
        String str1_3 = new String(str1, 0, 0, Charset.forName("ASCII"));
        String str1_4 = new String(str1, 5, 1, Charset.forName("ASCII"));
        System.out.println(str1_1);
        System.out.println(str1_2);
        System.out.println(str1_3);
        System.out.println(str1_4);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 123\nabc123\n\n3\n0\n