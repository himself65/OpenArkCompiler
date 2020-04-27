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
 * -@TestCaseID: ExcpReturn04150.java
 * -@TestCaseName: try{exception return_x}-catch（x){}-finally{return_v},-->caller(x)
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1:occur EH it try{} when return
 * -#step2:no catch in catch{}
 * -#step3:excute finally{}
 * -#step4:catch by caller
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ExcpReturn04150.java
 * -@ExecuteClass: ExcpReturn04150
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class ExcpReturn04150 {
    private static int processResult = 99;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    /**
     * main test fun
     *
     * @return status code
     */
    public static int run(String[] argv, PrintStream out) {
        int result = 2; /*STATUS_FAILED*/

        try {
            result = excpReturn04150();
        } catch (Exception e) {
            processResult -= 10;
        }

        if (result == 5 && processResult == 95) {
            result = 0;
        }

        return result;
    }

    /**
     * try{exception return_x}-catch（x){}-finally{return_v},-->caller(x)
     *
     * @return status code
     */
    public static int excpReturn04150() {
        int result1 = 4; /*STATUS_FAILED*/

        String str = "123#456";
        try {
            Integer.parseInt(str);
            return processResult;
        } catch (ClassCastException e) {
            System.out.println("=====See:ERROR!!!");
            result1 = 3;
            return result1;
        } catch (StringIndexOutOfBoundsException e) {
            System.out.println("=====See:ERROR!!!");
            result1 = 3;
            return result1;
        } catch (IllegalStateException e) {
            System.out.println("=====See:ERROR!!!");
            result1 = 3;
            return result1;
        } finally {
            processResult -= 4;
            result1++;
            return result1; // execute return 5
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n