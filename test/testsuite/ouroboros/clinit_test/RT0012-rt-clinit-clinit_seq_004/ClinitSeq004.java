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
 * -@TestCaseID: ClinitSeq004
 * -@TestCaseName: ClinitSeq004.java
 * -@RequirementName: Java class initialization
 * -@Title/Destination: 触发子接口初始化前不触发未初始化的包含default方法的超接口
 * -@Brief: 触发子接口初始化前不触发未初始化的包含default方法的超接口
 *  -#step1: 访问子接口字段,触发子接口的初始化，不触发父接口的初始化
 *  -#step2: 访问父接口字段，触发父接口初始化
 *  -#step3: 触发父接口中对象类的初始化
 *  -#step4:
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ClinitSeq004.java
 * -@ExecuteClass: ClinitSeq004
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

interface CaseInterface {
    int FIELD = ClinitSeq004.getCount();
    CaseOtherClass MEMBER = new CaseOtherClass();

    default int uselessFunction(int a, int b) {
        return a + b;
    }
}

interface CaseChild extends CaseInterface {
    int FIELD_CHILD = ClinitSeq004.getCount();
}

class CaseOtherClass {
    static int field;

    static {
        if (ClinitSeq004.getCount() == 3) {
            field = ClinitSeq004.getCount();
        }
    }
}

public class ClinitSeq004 {

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

        if (CaseChild.FIELD_CHILD != 1) {
            res = 2;
            out.println("Error, child not initialized");
        }

        if (CaseInterface.FIELD != 2) {
            res = 2;
            out.println("Error, interface not initialized");
        }

        if (CaseOtherClass.field != 4) {
            res = 2;
            out.println("Error, other not initialized");
        }
        return res;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full 0\n