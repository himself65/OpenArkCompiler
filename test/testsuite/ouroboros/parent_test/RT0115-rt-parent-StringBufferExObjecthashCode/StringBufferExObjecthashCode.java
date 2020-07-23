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
 * -@TestCaseID: StringBufferExObjecthashCode.java
 * -@TestCaseName: Exception in StringBuffer: int hashCode()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:
 * -#step1: Create Object1 by new StringBuffer(String name)
 * -#step2: Create Object2 is given by Object2, Object3 by new StringBuffer(String name)
 * -#step3: Check hashCode of the Object1 and Object2 is equal, check hashCode of the Object1 and Object3 is not equal.
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: StringBufferExObjecthashCode.java
 * -@ExecuteClass: StringBufferExObjecthashCode
 * -@ExecuteArgs:
 */

import java.lang.StringBuffer;

public class StringBufferExObjecthashCode {
    static int res = 99;

    public static void main(String argv[]) {
        System.out.println(new StringBufferExObjecthashCode().run());
    }

    /**
     * main test fun
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = stringBufferExObjecthashCode1();
        } catch (Exception e) {
            StringBufferExObjecthashCode.res = StringBufferExObjecthashCode.res - 20;
        }

        if (result == 4 && StringBufferExObjecthashCode.res == 89) {
            result = 0;
        }
        return result;
    }


    private int stringBufferExObjecthashCode1() {
        int result1 = 4; /*STATUS_FAILED*/
        // int hashCode()
        StringBuffer sb1 = new StringBuffer("aa");
        StringBuffer sb2 = sb1;
        StringBuffer sb3 = new StringBuffer("╬の〆");

        if (sb1.hashCode() == sb2.hashCode() && sb1.hashCode() != sb3.hashCode()) {
            StringBufferExObjecthashCode.res = StringBufferExObjecthashCode.res - 10;
        } else {
            StringBufferExObjecthashCode.res = StringBufferExObjecthashCode.res - 5;
        }
        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n