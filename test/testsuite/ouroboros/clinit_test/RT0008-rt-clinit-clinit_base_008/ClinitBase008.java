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
 * -@TestCaseID: ClinitBase008
 * -@TestCaseName: ClinitBase008.java
 * -@RequirementName: Java class initialization
 * -@Title/Destination: 针对"初始类"ClinitBase008，测试其能触发静态域的初始化
 * -@Brief: 针对"初始类"ClinitBase008，测试其能触发静态域的初始化
 *  -#step1: 针对"初始类"ClinitBase008，测试其能触发静态域的初始化
 *  -#step2:
 *  -#step3:
 *  -#step4:
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ClinitBase008.java
 * -@ExecuteClass: ClinitBase008
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

public class ClinitBase008 {
    static int res = 0;

    static {
        res = 1;
    }

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    public static int run(String[] argv, PrintStream out) {
        if (res == 1) {
            return 0;
        } else {
            return 1;
        }
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n