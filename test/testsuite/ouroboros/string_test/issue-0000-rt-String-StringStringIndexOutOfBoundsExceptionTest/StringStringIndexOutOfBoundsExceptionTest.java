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
 * -@TestCaseID: StringStringIndexOutOfBoundsExceptionTest.java
 * -@TestCaseName: Test UnsupportedEncodingException/StringIndexOutOfBoundsException occured in constructor of String(byte bytes[], int offset, int length, String charsetName)
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Give an invalid charset name for String constructor
 * -#step2: new instance use the String constructor
 * -#step3: Check UnsupportedEncodingException be thrown
 * -#step4: Give a negative number as offset for String constructor,then repeat step2,check StringIndexOutOfBoundsException be thrown
 * -#step4: Give a number larger than length for String constructor,then repeat step2,check StringIndexOutOfBoundsException be thrown
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: StringStringIndexOutOfBoundsExceptionTest.java
 * -@ExecuteClass: StringStringIndexOutOfBoundsExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;
import java.io.UnsupportedEncodingException;

public class StringStringIndexOutOfBoundsExceptionTest {
    private static int processResult = 99;

    public static void main(String argv[]) {
        System.out.println(run(argv, System.out));
    }
    public static int run(String argv[], PrintStream out) {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = StringStringIndexOutOfBoundsExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 4 && processResult == 96) {
            result = 0;
        }
//        System.out.println("result: " + result);
//        System.out.println("processResult:" + processResult);
        return result;
    }

    public static int StringStringIndexOutOfBoundsExceptionTest_1() throws UnsupportedEncodingException {
        int result = 4; /*STATUS_FAILED*/
        byte[] str1_1 = new byte[]{(byte) 0x61, (byte) 0x62, (byte) 0x63, (byte) 0x31, (byte) 0x32, (byte) 0x33};
        try {
            String str1 = new String(str1_1, 0, 3, "ASC");
            processResult -= 10;
        } catch (UnsupportedEncodingException e1) {
            processResult--;
        }
        try {
            String str2 = new String(str1_1, -1, 3, "ASCII");
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult -= 1;
        }
        try {
            String str3 = new String(str1_1, 0, 8, "ASCII");
            processResult -= 10;
        } catch (StringIndexOutOfBoundsException e1) {
            processResult--;
        }
        return result;
    }
}





// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n