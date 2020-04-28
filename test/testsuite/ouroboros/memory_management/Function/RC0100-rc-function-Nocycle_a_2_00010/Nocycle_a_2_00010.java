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
 * -@TestCaseID:maple/runtime/rc/function/Nocycle_a_2_00010.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: Nocycle_a_2_00010 in RC测试-No-Cycle-00.vsd.
 *- @Brief:functionTest
 *- @Expect:ExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: Nocycle_a_2_00010.java
 *- @ExecuteClass: Nocycle_a_2_00010
 *- @ExecuteArgs:
 */
class Nocycle_a_2_00010_A1 {
    Nocycle_a_2_00010_B1 b1_0;
    Nocycle_a_2_00010_B2 b2_0;
    Nocycle_a_2_00010_B3 b3_0;
    int a;
    int sum;
    String strObjectName;

    Nocycle_a_2_00010_A1(String strObjectName) {
        b1_0 = null;
        b2_0 = null;
        b3_0 = null;
        a = 101;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
    }

    void add() {
        sum = a + b1_0.a + b2_0.a + b3_0.a;
    }
}

class Nocycle_a_2_00010_B1 {
    int a;
    int sum;
    String strObjectName;

    Nocycle_a_2_00010_B1(String strObjectName) {
        a = 201;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
    }

    void add() {
        sum = a + a;
    }
}

class Nocycle_a_2_00010_B2 {
    int a;
    int sum;
    String strObjectName;

    Nocycle_a_2_00010_B2(String strObjectName) {
        a = 202;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B2_"+strObjectName);
    }

    void add() {
        sum = a + a;
    }
}

class Nocycle_a_2_00010_B3 {
    int a;
    int sum;
    String strObjectName;

    Nocycle_a_2_00010_B3(String strObjectName) {
        a = 203;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B3_"+strObjectName);
    }

    void add() {
        sum = a + a;
    }
}

class Nocycle_a_2_00010_A2 {
    int a;
    int sum;
    String strObjectName;

    Nocycle_a_2_00010_A2(String strObjectName) {
        a = 10;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
    }

    void add() {
        sum = a + a;
    }
}

public class Nocycle_a_2_00010 {
    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();

    }

    private static void rc_testcase_main_wrapper() {
        Nocycle_a_2_00010_A1 a1_main = new Nocycle_a_2_00010_A1("a1_main");
        a1_main.b1_0 = new Nocycle_a_2_00010_B1("b1_0");
        a1_main.b2_0 = new Nocycle_a_2_00010_B2("b2_0");
        a1_main.b3_0 = new Nocycle_a_2_00010_B3("b3_0");
        Nocycle_a_2_00010_A2 a2_main = new Nocycle_a_2_00010_A2("a2_main");
        a1_main.add();
        a1_main.b1_0.add();
        a1_main.b2_0.add();
        a1_main.b3_0.add();
        a2_main.add();
        int result = a1_main.sum + a1_main.b1_0.sum + a1_main.b2_0.sum + a1_main.b3_0.sum + a2_main.sum;
        if (result == 1939)
            System.out.println("ExpectResult");
    }

}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\nExpectResult\n