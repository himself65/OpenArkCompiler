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
 * -@TestCaseID: StringBuilderCapacityAndEnsureCapacityTest.java
 * -@TestCaseName: Test Method capacity()/ensureCapacity(int minimumCapacity) of StringBuilder.
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create StringBuilder instance.
 * -#step2: Test method int capacity(),check the result capacity and the length of instance is correctly.
 * -#step3: Create str(str.length > 0), Invoke Append(String str),test method capacity(), check the result
 *          capacity and the length of instance is correctly.
 * -#step4: Create minimumCapacity > old capacity(), Test method ensureCapacity(int minimumCapacity), test
 *          method capacity(), check the result capacity and the length of instance is correctly.
 * -#step5: Create newLength > StringBuffer.length, Invoke setLength(int newLength), test method capacity(), check
 *          the result capacity and the length of instance is correctly.
 * -#step6: Invoke trimToSize(), test method capacity(), check the result capacity and the length of instance is
 *          correctly.
 * -#step7: Create minimumCapacity = 0 or minimumCapacity < 0 or 0 < minimumCapacity < old capacity(), Test method
 *          ensureCapacity(int minimumCapacity), test method capacity(), check the result capacity and the length of
 *          instance is correctly.
 * -#step8: Change instance as One or more kinds of letters, numbers, special symbols/""/null to repeat step2~7.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringBuilderCapacityAndEnsureCapacityTest.java
 * -@ExecuteClass: StringBuilderCapacityAndEnsureCapacityTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringBuilderCapacityAndEnsureCapacityTest {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringBuilderCapacityAndEnsureCapacityTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringBuilderCapacityAndEnsureCapacityTest_1() {
        StringBuilder strBuilder1_1 = new StringBuilder("qwertyuiop{}[]\\|asdfghjkl;:'\"zxcvbnm,.<>/?~`1234567890" +
                "-=!@#$%^&*()_+ ASDFGHJKLQWERTYUIOPZXCVBNM0x96");
        StringBuilder strBuilder1_2 = new StringBuilder(" @!.&%()*");
        StringBuilder strBuilder1_3 = new StringBuilder("abc123");
        StringBuilder strBuilder1_4 = new StringBuilder("");
        StringBuilder strBuilder1_5 = new StringBuilder();

        test1(strBuilder1_1);
        test1(strBuilder1_2);
        test1(strBuilder1_3);
        test1(strBuilder1_4);
        test1(strBuilder1_5);
    }

    private static void test1(StringBuilder strBuilder) {
        System.out.println("1-capacity: " + strBuilder.capacity());
        System.out.println("1-lent: " + strBuilder.length());

        strBuilder = strBuilder.append("01234567890123456789");
        System.out.println("2-capacity: " + strBuilder.capacity());
        System.out.println("2-lent: " + strBuilder.length());

        strBuilder.ensureCapacity(80);
        System.out.println("3-capacity: " + strBuilder.capacity());
        System.out.println("3-lent: " + strBuilder.length());

        strBuilder.setLength(100);
        System.out.println("4-capacity: " + strBuilder.capacity());
        System.out.println("4-lent: " + strBuilder.length());

        strBuilder.trimToSize();
        System.out.println("5-capacity: " + strBuilder.capacity());
        System.out.println("5-lent: " + strBuilder.length());

        strBuilder.ensureCapacity(-2);
        System.out.println("6-capacity: " + strBuilder.capacity());
        System.out.println("6-lent: " + strBuilder.length());

        strBuilder.ensureCapacity(0);
        System.out.println("7-capacity: " + strBuilder.capacity());
        System.out.println("7-lent: " + strBuilder.length());

        strBuilder.ensureCapacity(2);
        System.out.println("8-capacity: " + strBuilder.capacity());
        System.out.println("8-lent: " + strBuilder.length());
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 1-capacity: 115\n1-lent: 99\n2-capacity: 232\n2-lent: 119\n3-capacity: 232\n3-lent: 119\n4-capacity: 232\n4-lent: 100\n5-capacity: 100\n5-lent: 100\n6-capacity: 100\n6-lent: 100\n7-capacity: 100\n7-lent: 100\n8-capacity: 100\n8-lent: 100\n1-capacity: 25\n1-lent: 9\n2-capacity: 52\n2-lent: 29\n3-capacity: 106\n3-lent: 29\n4-capacity: 106\n4-lent: 100\n5-capacity: 100\n5-lent: 100\n6-capacity: 100\n6-lent: 100\n7-capacity: 100\n7-lent: 100\n8-capacity: 100\n8-lent: 100\n1-capacity: 22\n1-lent: 6\n2-capacity: 46\n2-lent: 26\n3-capacity: 94\n3-lent: 26\n4-capacity: 190\n4-lent: 100\n5-capacity: 100\n5-lent: 100\n6-capacity: 100\n6-lent: 100\n7-capacity: 100\n7-lent: 100\n8-capacity: 100\n8-lent: 100\n1-capacity: 16\n1-lent: 0\n2-capacity: 34\n2-lent: 20\n3-capacity: 80\n3-lent: 20\n4-capacity: 162\n4-lent: 100\n5-capacity: 100\n5-lent: 100\n6-capacity: 100\n6-lent: 100\n7-capacity: 100\n7-lent: 100\n8-capacity: 100\n8-lent: 100\n1-capacity: 16\n1-lent: 0\n2-capacity: 34\n2-lent: 20\n3-capacity: 80\n3-lent: 20\n4-capacity: 162\n4-lent: 100\n5-capacity: 100\n5-lent: 100\n6-capacity: 100\n6-lent: 100\n7-capacity: 100\n7-lent: 100\n8-capacity: 100\n8-lent: 100\n0\n