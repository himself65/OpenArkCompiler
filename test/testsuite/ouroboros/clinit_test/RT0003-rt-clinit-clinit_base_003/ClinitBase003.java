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
 * -@TestCaseID: ClinitBase003
 * -@TestCaseName: ClinitBase003.java
 * -@RequirementName: Java class initialization
 * -@Title/Destination: 首次为类的静态字段赋值，触发类初始化
 * -@Brief: 首次为类的静态字段赋值，触发类初始化
 *  -#step1: 对局部类的内部类的静态字段赋值，触发此类的初始化
 *  -#step2: 对局部类的静态字段赋值，触发此类的初始化
 *  -#step3:
 *  -#step4:
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ClinitBase003.java
 * -@ExecuteClass: ClinitBase003
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

class CaseClass {
    static float field;

    static {
        if (ClinitBase003.getCount() == 3) {
            field = ClinitBase003.getCount();
        }
    }

    public static class StaticClass {
        public static float field;

        static {
            if (ClinitBase003.getCount() == 1) {
                field = ClinitBase003.getCount();
            }
        }
    }
}

public class ClinitBase003 {
    static private float count = 0;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    static float getCount() {
        count += 1.0f;
        return count;
    }

    private static int run(String[] argv, PrintStream out) {
        int res = 0;

        class CaseChild extends CaseClass {
            class CaseStaticChild extends StaticClass {
            }
        }
        // 对局部类的内部类的静态字段赋值，触发此类的初始化
        CaseChild.CaseStaticChild.field = 20.0f;
        // 对局部类的静态字段赋值，触发此类的初始化
        CaseChild.field = 10.0f;

        if (CaseClass.field != 10.0f) {
            res = 2;
            out.println("Error: CaseClass not initialized");
        }

        if (CaseClass.StaticClass.field != 20.0f) {
            out.println(CaseClass.StaticClass.field);
            res = 2;
            out.println("Error: CaseClass.StaticClass not initialized");
        }

        return res;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n