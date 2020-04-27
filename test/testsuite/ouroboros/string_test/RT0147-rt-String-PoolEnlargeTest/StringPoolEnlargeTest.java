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
 * -@TestCaseID: StringPoolEnlargeTest.java
 * -@TestCaseName: Test mapleStringPool large and large by public String intern().
 * -@TestCaseType: Function Test
 * -@RequirementName: Java字符串实现
 * -@Brief:
 * -#step1: Create String instance by Integer.toHexString(int index), index cover 0 to 47000.
 * -#step2: Test method intern(), check get result correctly.
 * -#step3: Change String instance create by Integer.toHexString(int index), index cover 0 to 470000, repeat step 2.
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: StringPoolEnlargeTest.java
 * -@ExecuteClass: StringPoolEnlargeTest
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class StringPoolEnlargeTest {
    private static int processResult = 99;
    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        int result = 2;  /* STATUS_Success */
        try {
            result = result - StringPoolEnlargeTest_1(47000);
            result = result - StringPoolEnlargeTest_1(470000);
        } catch (Exception e) {
            System.out.println(e);
            StringPoolEnlargeTest.processResult = StringPoolEnlargeTest.processResult - 10;
        }

        if (result == 0 && StringPoolEnlargeTest.processResult == 95) {
            result = 0;
        }
        return result;
    }

    public static int StringPoolEnlargeTest_1(int len) {
        int length = len; // 47 mapleStringPool/process, 1000 key-value/mapleStringPool
        try {
            String[] s;
            s = new String[length];
            for (int i = 0; i < length; i++) {
                s[i] = Integer.toHexString(i);
                if (s[i].intern() == Integer.toHexString(func1(len-1)).intern()) {
                    StringPoolEnlargeTest.processResult = StringPoolEnlargeTest.processResult - 2;
                    return 1;
                }
            }
        } catch (OutOfMemoryError e) {
            return 114;
        }
        StringPoolEnlargeTest.processResult = StringPoolEnlargeTest.processResult - 10;
        return 102;
    }

    public static int func1(int i) {
        return i;
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n