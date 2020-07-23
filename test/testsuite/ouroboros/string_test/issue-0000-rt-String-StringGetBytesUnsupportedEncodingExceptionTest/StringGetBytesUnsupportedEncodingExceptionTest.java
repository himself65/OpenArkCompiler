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
 * -@TestCaseID: StringGetBytesUnsupportedEncodingExceptionTest.java
 * -@TestCaseName: Test UnsupportedEncodingException occured in getBytes(String charsetName)
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Give an invalid CharsetName as args
 * -#step2: Invoke Method of getBytes
 * -#step3: Check UnsupportedEncodingException be thrown by Method(getBytes)
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: StringGetBytesUnsupportedEncodingExceptionTest.java
 * -@ExecuteClass: StringGetBytesUnsupportedEncodingExceptionTest
 * -@ExecuteArgs:
 */



import java.io.PrintStream;
import java.io.UnsupportedEncodingException;

public class StringGetBytesUnsupportedEncodingExceptionTest {
    private static int processResult = 99;

    public static void main(String argv[]) {
        System.out.println(run(argv, System.out));
    }
    public static int run(String argv[], PrintStream out) {
        int result = 3; /*STATUS_FAILED*/
        try {
            result = StringGetBytesUnsupportedEncodingExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }
        if (result == 2 && processResult == 98) {
            result = 0;
        }
//        System.out.println("result: " + result);
//        System.out.println("processResult:" + processResult);
        return result;
    }

    public static int StringGetBytesUnsupportedEncodingExceptionTest_1() {
        int result1 = 3; /*STATUS_FAILED*/
        String str1_1 = new String("abc123");
//        String str1_2 = new String("      ");
//        String str1_3 = new String("abc123");
        String str1_4 = new String("");
        String str1_5 = new String();

        String str2_1 = "abc123";
//        String str2_2 = "      ";
//        String str2_3 = "abc123";
        String str2_4 = "";
//        String str2_5 = null;
        try {
            byte[] test1_1 = str1_1.getBytes("asc");
            processResult -= 10;
        } catch (UnsupportedEncodingException e1) {
            processResult--;
        }
        return 2;
    }
}





// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n