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
 * -@TestCaseID: StringSubstringStringIndexOutOfBoundsExceptionTest.java
 * -@TestCaseName: Test StringIndexOutOfBoundsException in String Method: String substring(int beginIndex).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance.
 * -#step2: Create Parameters: beginIndex range from -1 to max (max > instance.length).
 * -#step3: Test method substring(int beginIndex), Check result is correctly. StringIndexOutOfBoundsException is thrown
 *          when beginIndex < 0 or beginIndex > instance.length.
 * -#step4: Change instance as One or more kinds of letters, numbers, special symbols/""/NoParam to repeat step2~3.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringSubstringStringIndexOutOfBoundsExceptionTest.java
 * -@ExecuteClass: StringSubstringStringIndexOutOfBoundsExceptionTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringSubstringStringIndexOutOfBoundsExceptionTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringSubstringStringIndexOutOfBoundsExceptionTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringSubstringStringIndexOutOfBoundsExceptionTest_1() {
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
        String str2_4 = "";

        test(str1_1);
        test(str1_2);
        test(str1_3);
        test(str1_4);
        test(str1_5);

        test(str2_1);
        test(str2_2);
        test(str2_3);
        test(str2_4);
    }

    private static void test(String str) {
        for (int i = -1; i < 8; i++) {
            try {
                System.out.println("beginIndex= " + i);
                String dst1_1 = str.substring(i);
                System.out.println(dst1_1);
            } catch (StringIndexOutOfBoundsException e1) {
                System.out.println("StringIndexOutOfBounds 1_1");
            }
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full beginIndex= -1\nStringIndexOutOfBounds 1_1\nbeginIndex= 0\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nbeginIndex= 1\nwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nbeginIndex= 2\nertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nbeginIndex= 3\nrtyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nbeginIndex= 4\ntyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nbeginIndex= 5\nyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nbeginIndex= 6\nuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nbeginIndex= 7\niop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nbeginIndex= -1\nStringIndexOutOfBounds 1_1\nbeginIndex= 0\n @!.&%\nbeginIndex= 1\n@!.&%\nbeginIndex= 2\n!.&%\nbeginIndex= 3\n.&%\nbeginIndex= 4\n&%\nbeginIndex= 5\n%\nbeginIndex= 6\n\nbeginIndex= 7\nStringIndexOutOfBounds 1_1\nbeginIndex= -1\nStringIndexOutOfBounds 1_1\nbeginIndex= 0\nabc123\nbeginIndex= 1\nbc123\nbeginIndex= 2\nc123\nbeginIndex= 3\n123\nbeginIndex= 4\n23\nbeginIndex= 5\n3\nbeginIndex= 6\n\nbeginIndex= 7\nStringIndexOutOfBounds 1_1\nbeginIndex= -1\nStringIndexOutOfBounds 1_1\nbeginIndex= 0\n\nbeginIndex= 1\nStringIndexOutOfBounds 1_1\nbeginIndex= 2\nStringIndexOutOfBounds 1_1\nbeginIndex= 3\nStringIndexOutOfBounds 1_1\nbeginIndex= 4\nStringIndexOutOfBounds 1_1\nbeginIndex= 5\nStringIndexOutOfBounds 1_1\nbeginIndex= 6\nStringIndexOutOfBounds 1_1\nbeginIndex= 7\nStringIndexOutOfBounds 1_1\nbeginIndex= -1\nStringIndexOutOfBounds 1_1\nbeginIndex= 0\n\nbeginIndex= 1\nStringIndexOutOfBounds 1_1\nbeginIndex= 2\nStringIndexOutOfBounds 1_1\nbeginIndex= 3\nStringIndexOutOfBounds 1_1\nbeginIndex= 4\nStringIndexOutOfBounds 1_1\nbeginIndex= 5\nStringIndexOutOfBounds 1_1\nbeginIndex= 6\nStringIndexOutOfBounds 1_1\nbeginIndex= 7\nStringIndexOutOfBounds 1_1\nbeginIndex= -1\nStringIndexOutOfBounds 1_1\nbeginIndex= 0\nqwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nbeginIndex= 1\nwertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nbeginIndex= 2\nertyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nbeginIndex= 3\nrtyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nbeginIndex= 4\ntyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nbeginIndex= 5\nyuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nbeginIndex= 6\nuiop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nbeginIndex= 7\niop{}[]\|asdfghjkl;:'"zxcvbnm,.<>/?~`1234567890-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96\nbeginIndex= -1\nStringIndexOutOfBounds 1_1\nbeginIndex= 0\n @!.&%\nbeginIndex= 1\n@!.&%\nbeginIndex= 2\n!.&%\nbeginIndex= 3\n.&%\nbeginIndex= 4\n&%\nbeginIndex= 5\n%\nbeginIndex= 6\n\nbeginIndex= 7\nStringIndexOutOfBounds 1_1\nbeginIndex= -1\nStringIndexOutOfBounds 1_1\nbeginIndex= 0\nabc123\nbeginIndex= 1\nbc123\nbeginIndex= 2\nc123\nbeginIndex= 3\n123\nbeginIndex= 4\n23\nbeginIndex= 5\n3\nbeginIndex= 6\n\nbeginIndex= 7\nStringIndexOutOfBounds 1_1\nbeginIndex= -1\nStringIndexOutOfBounds 1_1\nbeginIndex= 0\n\nbeginIndex= 1\nStringIndexOutOfBounds 1_1\nbeginIndex= 2\nStringIndexOutOfBounds 1_1\nbeginIndex= 3\nStringIndexOutOfBounds 1_1\nbeginIndex= 4\nStringIndexOutOfBounds 1_1\nbeginIndex= 5\nStringIndexOutOfBounds 1_1\nbeginIndex= 6\nStringIndexOutOfBounds 1_1\nbeginIndex= 7\nStringIndexOutOfBounds 1_1\n0\n

