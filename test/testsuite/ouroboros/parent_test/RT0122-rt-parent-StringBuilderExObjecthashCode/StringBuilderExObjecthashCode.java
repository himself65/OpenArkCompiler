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
 * -@TestCaseID: StringBuilderExObjecthashCode.java
 * -@TestCaseName: Exception in StringBuilder:   int hashCode()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create Object1 by new StringBuilder(String name)
 * -#step2: Create Object2 is given by Object2, Object3 by new StringBuilder(String name)
 * -#step3: Check hashCode of the Object1 and Object2 is equal, check hashCode of the Object1 and Object3 is not equal.
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: StringBuilderExObjecthashCode.java
 * -@ExecuteClass: StringBuilderExObjecthashCode
 * -@ExecuteArgs:
 */

import java.lang.StringBuilder;

public class StringBuilderExObjecthashCode {
    static int res = 99;

    public static void main(String argv[]) {
        System.out.println(new StringBuilderExObjecthashCode().run());
    }

    /**
     * main test fun
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = stringBuilderExObjecthashCode1();
        } catch (Exception e) {
            StringBuilderExObjecthashCode.res = StringBuilderExObjecthashCode.res - 20;
        }

        if (result == 4 && StringBuilderExObjecthashCode.res == 89) {
            result = 0;
        }

        return result;
    }


    private int stringBuilderExObjecthashCode1() {
        int result1 = 4; /*STATUS_FAILED*/
        //   int hashCode()

        StringBuilder sb1 = new StringBuilder("aa");
        StringBuilder sb2 = sb1;
        StringBuilder sb3 = new StringBuilder("╬の〆");

        if (sb1.hashCode() == sb2.hashCode() && sb1.hashCode() != sb3.hashCode()) {
            StringBuilderExObjecthashCode.res = StringBuilderExObjecthashCode.res - 10;
        } else {
            StringBuilderExObjecthashCode.res = StringBuilderExObjecthashCode.res - 5;
        }
        return result1;
    }
}




// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n