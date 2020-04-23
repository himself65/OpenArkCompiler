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
 * -@TestCaseID: ClinitSeq003
 * -@TestCaseName: ClinitSeq003.java
 * -@RequirementName: Java class initialization
 * -@Title/Destination: 触发子类初始化前优先触发未初始化的包含default方法的超接口与继承的父类
 * -@Brief: 触发子类初始化前优先触发未初始化的包含default方法的超接口与继承的父类
 *  -#step1: 访问CaseChild触发父类初始化
 *  -#step2: 接着触发接口初始化
 *  -#step3: 再触发接口字段中对象的类的初始化
 *  -#step4: 最后触发自己的初始化
 * -@Expect: 0\n
 * -@Priority: High
 * -@Source: ClinitSeq003.java
 * -@ExecuteClass: ClinitSeq003
 * -@ExecuteArgs:
 */

import java.io.PrintStream;

interface CaseInterface {
    int FIELD = ClinitSeq003.getCount();
    CaseOtherClass MEMBER = new CaseOtherClass();

    default int uselessFunction(int a, int b) {
        return a + b;
    }
}

class CaseOtherClass {
    static int field;

    static {
        if (ClinitSeq003.getCount() == 4) {
            field = ClinitSeq003.getCount();
        }
    }
}

class CaseFather {
    static int field;

    static {
        if (ClinitSeq003.getCount() == 1) {
            field = ClinitSeq003.getCount();
        }
    }
}

class CaseChild extends CaseFather implements CaseInterface {
    static int field;

    static {
        if (ClinitSeq003.getCount() == 6) {
            field = ClinitSeq003.getCount();
        }
    }
}

public class ClinitSeq003 {

    static private int count = 0;

    public static void main(String[] argv) {
        System.out.println(run(argv, System.out) /*STATUS_TEMP*/);
    }

    static int getCount() {
        count += 1;
        return count;
    }

    private static int run(String[] argv, PrintStream out) {
        int res = 0;

        // 访问子类的字段
        if (CaseChild.field != 7) {
            res = 2;
            out.println(CaseChild.field);
            out.println("Error, child not initialized");
        }

        // 接口中的字段对应的类被触发初始化
        if (CaseOtherClass.field != 5) {
            res = 2;
            out.println("Error, other not initialized");
        }

        // 包含有default方法的接口被触发初始化
        if (CaseInterface.FIELD != 3) {
            res = 2;
            out.println("Error, interface not initialized");
        }

        // 父类被触发初始化
        if (CaseFather.field != 2) {
            res = 2;
            out.println("Error, father not initialized");
        }
        return res;
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan 0\n