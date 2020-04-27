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
 * -@TestCaseID: StringUserCaseLiteralsISO88591Test.java
 * -@TestCaseName: Literals is ISO-8859-1,new String instance correctly
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Prepare a String which literals is ISO-8859-1
 * -#step2: Get Its Bytes
 * -#step3: New String instance with Bytes and ISO-8859-1 charset
 * -#step4: Check the String is correct
 * -@Expect:expected.txt
 * -@Priority: High
 * -@Source: StringUserCaseLiteralsISO88591Test.java
 * -@ExecuteClass: StringUserCaseLiteralsISO88591Test
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.io.UnsupportedEncodingException;
import java.nio.charset.Charset;

public class StringUserCaseLiteralsISO88591Test {
    private static int processResult = 99;

    public static void main(String[] argv) {
        run(argv, System.out);
    }
    public static int run(String argv[], PrintStream out) {
        int result = 2/*STATUS_FAILED*/;
        try {
            StringUserCaseLiteralsISO88591Test_1();
        } catch (Exception e) {
            System.out.println(e);
            processResult = processResult - 10;
        }
//        System.out.println("result: " + result);
//        System.out.println("processResult:" + processResult);
        if (result == 1 && processResult == 99) {
            result =0;
        }
        return result;
    }

    public static void StringUserCaseLiteralsISO88591Test_1() throws UnsupportedEncodingException {
        String testCaseID = "StringUserCaseLiteralsISO88591Test_1";
        System.out.println("========================" + testCaseID);
        //0x00~0x7F
        String str1_1 = "a b^c~AB";
        //10
        char data1_2[] = {97, 32, 98, 94, 99, 126, 65, 66};
        String str1_2 = new String(data1_2);
        //16
//        char data1_3[] = {0x61,0x20, 0x62, 0x5E, 0x63, 0x7E,0x00, 0x41, 0x42, 0x43,0x7F};
        char data1_3[] = {0x61, 0x20, 0x62, 0x5E, 0x63, 0x7E, 0x41, 0x42};
        String str1_3 = new String(data1_3);

//        //0x80~0xFF
        String str2_1 = "jag ls dig";
//        //16 ISO-8859-1 0x80undefinit ,
        char data2_3[] = {0x80, 0xC5, 0xFF, 0xD7, 0xF7};
        String str2_3 = new String(data2_3);

        byte[] data2_4 = {(byte) 0x80, (byte) 0xC5, (byte) 0xFF, (byte) 0xD7, (byte) 0xF7};
        String str2_4 = new String(data2_4, Charset.forName("iso-8859-1"));

        byte[] testb = str2_1.getBytes("ISO-8859-1");
        String tests = new String(testb, "ISO-8859-1");
        test1(tests);

    }

    private static void test1(String str) {
        System.out.println(str);
//        for (int i=0;i< str.length() ; i++) {
//            char ch = str.charAt(i);
////            System.out.print("char: " + ch + " unicode: ");
//            System.out.println(Integer.toHexString(ch));
//
//        }
    }

}




// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan \=\=\=\=\=\=\=\=\=\=\=\=\=\=\=\=\=\=\=\=\=\=\=\=StringUserCaseLiteralsISO88591Test_1\s*jag\s*ls\s*dig