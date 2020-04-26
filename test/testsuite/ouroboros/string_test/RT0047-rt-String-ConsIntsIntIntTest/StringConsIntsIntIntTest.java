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
 * -@TestCaseID: StringConsIntsIntIntTest.java
 * -@TestCaseName: Test String constructor: String(int[] codePoints, int offset, int count).
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create Parameters: codePoints.length > 0, value.element is Decimal number.
 * -#step2: Create Parameters: offset = 0, count = codePoints.length.
 * -#step3: Test method String(int[] codePoints, int offset, int count), check the return String is correctly.
 * -#step4: Create Parameters: offset = codePoints.length - 1, count = 1.
 * -#step5: Test method String(int[] codePoints, int offset, int count), check the return String is correctly.
 * -#step6: Create Parameters: offset = codePoints.length - 1, count = 0.
 * -#step7: Test method String(int[] codePoints, int offset, int count), check the return String is correctly.
 * -#step8: Create Parameters: codePoints.length > 0, value.element is Hexadecimal number.
 * -#step9: Create Parameters: offset = 0, count = codePoints.length.
 * -#step10: Test method String(int[] codePoints, int offset, int count), check the return String is correctly.
 * -#step11: Create Parameters: offset = codePoints.length - 1, count = 1.
 * -#step12: Test method String(int[] codePoints, int offset, int count), check the return String is correctly.
 * -#step13: Create Parameters: offset = codePoints.length - 1, count = 0.
 * -#step14: Test method String(int[] codePoints, int offset, int count), check the return String is correctly.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringConsIntsIntIntTest.java
 * -@ExecuteClass: StringConsIntsIntIntTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringConsIntsIntIntTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringConsIntsIntIntTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringConsIntsIntIntTest_1() {
        int[] charIntArray1_1 = new int[]{97, 98, 99, 49, 50, 51};
        String str1_1 = new String(charIntArray1_1, 0, charIntArray1_1.length);
        String str1_2 = new String(charIntArray1_1, charIntArray1_1.length - 1, 1);
        String str1_3 = new String(charIntArray1_1, charIntArray1_1.length - 1, 0);
        System.out.println(str1_1);
        System.out.println(str1_2);
        System.out.println(str1_3);

        int[] charIntArray2_1 = new int[]{0x61, 0x62, 0x63, 0x31, 0x32, 0x33};
        String str2_1 = new String(charIntArray2_1, 0, charIntArray2_1.length);
        String str2_2 = new String(charIntArray2_1, charIntArray2_1.length - 1, 1);
        String str2_3 = new String(charIntArray2_1, charIntArray2_1.length - 1, 0);
        System.out.println(str2_1);
        System.out.println(str2_2);
        System.out.println(str2_3);
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan abc123\s*3\s*abc123\s*3\s*0