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
 * -@TestCaseID:maple/runtime/rc/function/Nocycle_a_00120.java
 * -@TestCaseName:MyselfClassName
 * -@RequirementName:[运行时需求]支持自动内存管理
 * -@Title/Destination: Nocycle_a_00120 in RC测试-No-Cycle-00.vsd.
 * -@Brief:functionTest
 * -@Expect:ExpectResult\n
 * -@Priority: High
 * -@Source: Nocycle_a_00120.java
 * -@ExecuteClass: Nocycle_a_00120
 * -@ExecuteArgs:
 */
class Nocycle_a_00120_A1 {
    Nocycle_a_00120_B1 b1_0;
    int a;
    int sum;
    String strObjectName;

    Nocycle_a_00120_A1(String strObjectName) {
        b1_0 = null;
        a = 101;
        sum = 0;
        this.strObjectName = strObjectName;
    }

    void add() {
        sum = a + b1_0.a;
    }
}

class Nocycle_a_00120_A2 {
    Nocycle_a_00120_B1 b1_0;
    int a;
    int sum;
    String strObjectName;

    Nocycle_a_00120_A2(String strObjectName) {
        b1_0 = null;
        a = 102;
        sum = 0;
        this.strObjectName = strObjectName;
    }

    void add() {
        sum = a + b1_0.a;
    }
}

class Nocycle_a_00120_B1 {
    int a;
    int sum;
    String strObjectName;

    Nocycle_a_00120_B1(String strObjectName) {
        a = 201;
        sum = 0;
        this.strObjectName = strObjectName;
    }

    void add() {
        sum = a + a;
    }
}

public class Nocycle_a_00120 {

    public static void main(String[] args) {
        Nocycle_a_00120_A1 a1_main = new Nocycle_a_00120_A1("a1_main");
        Nocycle_a_00120_A2 a2_main = new Nocycle_a_00120_A2("a2_main");
        a1_main.b1_0 = new Nocycle_a_00120_B1("b1_0");
        a2_main.b1_0 = new Nocycle_a_00120_B1("b1_0");
        a1_main.add();
        a2_main.add();
        a1_main.b1_0.add();
        a2_main.b1_0.add();

        int result = a1_main.sum + a2_main.sum + a1_main.b1_0.sum;
        if (result == 1007) {
            System.out.println("ExpectResult");
        }
    }
}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n