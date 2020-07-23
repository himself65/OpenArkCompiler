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
 * -@TestCaseID: StringUnsupportedEncodingExceptionTest.java
 * -@TestCaseName: Test UnsupportedEncodingException occured in String(byte bytes[], String charsetName)
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Give an invalid charset name for String constructor
 * -#step2: new instance use the String constructor
 * -#step3: Check UnsupportedEncodingException be thrown
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: StringUnsupportedEncodingExceptionTest.java
 * -@ExecuteClass: StringUnsupportedEncodingExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.io.UnsupportedEncodingException;

public class StringUnsupportedEncodingExceptionTest {
    private static int processResult = 99;

    public static void main(String argv[]) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String argv[], PrintStream out) {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = StringUnsupportedEncodingExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 4 && processResult == 98) {
            result = 0;
        }
//        System.out.println("result: " + result);
//        System.out.println("processResult:" + processResult);
        return result;
    }

    public static int StringUnsupportedEncodingExceptionTest_1() {
        int result1 = 4; /*STATUS_FAILED*/
        //UnsupportedEncodingException - If the named charset is not supported
        //
        // byte[] str1_1 = new byte[]{(byte)0x61, (byte)0x62, (byte)0x63, (byte)0x31, (byte)0x32, (byte)0x33};
        //char[] str1_1 = {'a','b','c','1','2','3'};
        //int[] str1_2 = new int[]{0x61,0x62,0x63,0x31,0x32,0x33};
        //int[] str1_1 = new int[]{97,98,99,49,50,51};
        byte[] str1_1 = new byte[]{(byte) 0x61, (byte) 0x62, (byte) 0x63, (byte) 0x31, (byte) 0x32, (byte) 0x33};
        try {
            String str1 = new String(str1_1, "ASC");
            processResult -= 10;
        } catch (UnsupportedEncodingException e1) {
            processResult--;
        }
        return result1;
    }
}





// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n