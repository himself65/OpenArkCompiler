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
 * -@TestCaseID: StringGenericTest.java
 * -@TestCaseName: User Scene: <T>
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create ArrayList instance.
 * -#step2: Add letters or numbers to the ArrayList.
 * -#step3: Check the object belong to class java.util.ArrayList.
 * -#step4: Check if return value is the second param of the template function.
 * -@Expect: expected.txt
 * -@Priority: High
 * -@Source: StringGenericTest.java
 * -@ExecuteClass: StringGenericTest
 * -@ExecuteArgs:
 */

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.io.PrintStream;

public class StringGenericTest {
    private static int processResult = 99;

    public static void main(String[] argv) throws IllegalAccessException, IllegalArgumentException,
            InvocationTargetException, NoSuchMethodException, SecurityException {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2; /* STATUS_Success */

        try {
            StringGenericTest_1();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 99) {
            result = 0;
        }
        return result;
    }

    public static void StringGenericTest_1() throws IllegalAccessException, IllegalArgumentException,
            InvocationTargetException, NoSuchMethodException, SecurityException {
        ArrayList<String> arrayList1 = new ArrayList<String>();
        arrayList1.add("abc");

        ArrayList<Integer> arrayList2 = new ArrayList<Integer>();
        arrayList2.add(123);

        System.out.println(arrayList1.getClass() == arrayList2.getClass());

        String str = StringGenericTest.<String>add("1", "2");
        System.out.println(str);
    }

    public static <T> T add(T x, T y) {
        return y;
    }
}



// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full true\n2\n0\n