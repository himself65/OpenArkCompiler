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
 * -@TestCaseID: ThreadGroupExObjecthashCode.java
 * -@TestCaseName: Exception in ThreadGroup: int hashCode()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:Test api hashCode extends from Object
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ThreadGroupExObjecthashCode.java
 * -@ExecuteClass: ThreadGroupExObjecthashCode
 * -@ExecuteArgs:
 */

import java.lang.ThreadGroup;

public class ThreadGroupExObjecthashCode {
    static int res = 99;

    public static void main(String argv[]) {
        System.out.println(new ThreadGroupExObjecthashCode().run());
    }

    /**
     * main test fun
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = threadGroupExObjecthashCode1();
        } catch (Exception e) {
            ThreadGroupExObjecthashCode.res = ThreadGroupExObjecthashCode.res - 20;
        }

        if (result == 4 && ThreadGroupExObjecthashCode.res == 89) {
            result = 0;
        }

        return result;
    }

    private int threadGroupExObjecthashCode1() {
        int result1 = 4; /*STATUS_FAILED*/
        // int hashCode()

        ThreadGroup gr1 = new ThreadGroup("Thread8023");
        ThreadGroup gr2 = gr1;
        ThreadGroup gr3 = new ThreadGroup("Thread1988");

        if (gr1.hashCode() == gr2.hashCode() && gr1.hashCode() != gr3.hashCode()) {
            ThreadGroupExObjecthashCode.res = ThreadGroupExObjecthashCode.res - 10;
        } else {
            ThreadGroupExObjecthashCode.res = ThreadGroupExObjecthashCode.res - 5;
        }

        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n