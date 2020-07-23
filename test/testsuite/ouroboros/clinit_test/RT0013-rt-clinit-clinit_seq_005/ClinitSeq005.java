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
 * -@TestCaseID: ClinitSeq005
 * -@TestCaseName: ClinitSeq005.java
 * -@RequirementName: Java class initialization
 * -@Title/Destination: 触发子类初始化前不触发未初始化的不包含default方法的超接口
 * -@Brief: 触发子类初始化前不触发未初始化的包含default方法的超接口
 *  -#step1: 触发子类初始化前不触发未初始化的包含default方法的超接口
 *  -#step2: 通过访问接口对象的类字段，优先触发接口初始化，再触发接口对象的类初始化
 *  -#step3:
 *  -#step4:
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ClinitSeq005.java
 * -@ExecuteClass: ClinitSeq005
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

interface CaseInterface {
    int FIELD = ClinitSeq005.getCount();
    CaseOtherClass MEMBER = new CaseOtherClass();

    int uselessFunction(int a, int b);
}

class CaseOtherClass {
    static int field;

    static {
        if (ClinitSeq005.getCount() == 4) {
            field = ClinitSeq005.getCount();
        }
    }
}

class CaseChild implements CaseInterface {
    static int field;

    static {
        if (ClinitSeq005.getCount() == 1) {
            field = ClinitSeq005.getCount();
        }
    }

    public int uselessFunction(int a, int b) {
        return a + b;
    }
}

public class ClinitSeq005 {

    static private int count = 0;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out));
    }

    static int getCount() {
        count += 1;
        return count;
    }

    private static int run(String[] argv, PrintStream out) {
        int res = 0;

        if (CaseChild.field != 2) {
            res = 2;
            out.println("Error, child not initialized");
        }

        if (CaseInterface.MEMBER.field != 5) {
            res = 2;
            out.println("Error, other not initialized");
        }

        if (CaseInterface.FIELD != 3) {
            res = 2;
            out.println("Error, interface not initialized");
        }
        return res;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n