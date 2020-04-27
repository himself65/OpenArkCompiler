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
 * -@TestCaseID: ThreadLocalExObjecthashCode.java
 * -@TestCaseName: Exception in ThreadLocal: int hashCode()
 * -@TestCaseType: Function Test
 * -@RequirementName: 补充重写类的父类方法
 * -@Brief:Test ThreadLocal api hashCode extends from Object
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ThreadLocalExObjecthashCode.java
 * -@ExecuteClass: ThreadLocalExObjecthashCode
 * -@ExecuteArgs:
 */

import java.lang.ThreadLocal;

public class ThreadLocalExObjecthashCode {
    static int res = 99;

    public static void main(String argv[]) {
        System.out.println(new ThreadLocalExObjecthashCode().run());
    }

    /**
     * main test fun
     * @return status code
     */
    public int run() {
        int result = 2; /*STATUS_FAILED*/
        try {
            result = threadLocalExObjecthashCode1();
        } catch (Exception e) {
            ThreadLocalExObjecthashCode.res = ThreadLocalExObjecthashCode.res - 20;
        }

        if (result == 4 && ThreadLocalExObjecthashCode.res == 89) {
            result = 0;
        }

        return result;
    }

    private int threadLocalExObjecthashCode1() {
        int result1 = 4; /*STATUS_FAILED*/
        // int hashCode()

        ThreadLocal<Object> threadLocal1 = new ThreadLocal<Object>();
        ThreadLocal<Object> threadLocal2 = threadLocal1;
        ThreadLocal<Object> threadLocal3 = new ThreadLocal<Object>();

        if (threadLocal1.hashCode() == threadLocal2.hashCode() && threadLocal1.hashCode() != threadLocal3.hashCode()) {
            ThreadLocalExObjecthashCode.res = ThreadLocalExObjecthashCode.res - 10;
        } else {
            ThreadLocalExObjecthashCode.res = ThreadLocalExObjecthashCode.res - 5;
        }

        return result1;
    }
}
// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n