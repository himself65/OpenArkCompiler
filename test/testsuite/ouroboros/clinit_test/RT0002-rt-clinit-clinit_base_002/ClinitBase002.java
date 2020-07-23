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
 * -@TestCaseID: ClinitBase002
 * -@TestCaseName: ClinitBase002.java
 * -@RequirementName: Java class initialization
 * -@Title/Destination: 首次调用类的静态方法，触发类初始化
 * -@Brief: 首次调用类的静态方法，触发类初始化
 *  -#step1: 创建继承内部静态类的局部类
 *  -#step2: 访问局部类的静态方法，触发父类初始化
 *  -#step3: 创建继承外部类的局部类
 *  -#step4: 访问局部类的静态方法，触发父类初始化
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ClinitBase002.java
 * -@ExecuteClass: ClinitBase002
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

abstract class CaseClass {
    static float field;

    static {
        if (ClinitBase002.getCount() == 3) {
            field = ClinitBase002.getCount();
        }
    }

    static float getField() {
        return (float) (field * 1.2);
    }

    public abstract int uselessFunction();

    static class StaticClass {
        static float field;

        static {
            if (ClinitBase002.getCount() == 1) {
                field = ClinitBase002.getCount();
            }
        }

        static float getField() {
            return (float) (field * 1.2);
        }
    }
}

public class ClinitBase002 {
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

        // 创建继承内部静态类的局部类
        class AnonymousClass extends CaseClass.StaticClass {
        }
        // 访问局部类的静态方法，触发父类初始化
        if (AnonymousClass.getField() != 2.4f) {
            res = 2;
            out.println("Error: CaseClass.StaticClass not initialized");
        }

        // 创建继承外部类的局部类
        class ClassChild extends CaseClass {
            public int uselessFunction() {
                return 0;
            }
        }
        // 访问局部类的静态方法，触发父类初始化
        if (ClassChild.getField() != 4.8f) {
            res = 2;
            out.println("Error: CaseClass not initialized");
        }

        return res;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n