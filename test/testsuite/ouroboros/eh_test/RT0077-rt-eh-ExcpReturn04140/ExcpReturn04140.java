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
 * -@TestCaseID: ExcpReturn04140.java
 * -@TestCaseName: try{exception return_x}-catch（e){}-finally{},return_v
 * -@TestCaseType: Function Test
 * -@RequirementName: [运行时需求]支持Java异常处理
 * -@Brief:
 * -#step1:occur exception in try{}
 * -#step2:catch by catch{}
 * -#step3:excute finally{}
 * -#step4:excute return{}
 * -@Expect:0\n
 * -@Priority: High
 * -@Source: ExcpReturn04140.java
 * -@ExecuteClass: ExcpReturn04140
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class ExcpReturn04140 {
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
            result = excpReturn041401();
        } catch (NumberFormatException e) {
            processResult -= 10;
        }

        if (result == 2 && processResult == 95) {
            result = 0;
        }

        return result;
    }

    /**
     * try{exception return_x}-catch（e){}-finally{},return_v
     *
     * @return status code
     */
    public static int excpReturn041401() {
        int result1 = 4; /*STATUS_FAILED*/

        String str = "123#456";
        try {
            Integer.parseInt(str);
            return processResult;
        } catch (ClassCastException e) {
            System.out.println("=====See:ERROR!!!");
            result1 = 3;
        } catch (NumberFormatException e) {
            processResult--;
            result1 = 1;
        } catch (IllegalStateException e) {
            System.out.println("=====See:ERROR!!!");
            result1 = 3;
        } finally {
            processResult--;
            result1++;
        }
        processResult -= 2;
        return result1;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n