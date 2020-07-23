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
 * -@TestCaseID:maple/runtime/rc/function/Cycle_a_00210.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:Cycle_a_00210 in RC测试-Cycle-01
 *- @Brief:functionTest
 *- @Expect:ExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: Cycle_a_00210.java
 *- @ExecuteClass: Cycle_a_00210
 *- @ExecuteArgs:
 * A1 depend A2;  A4 depend A3 ;
 * A2 depend A3; A3 depend A1 A5 A6; A5 depend A4; A6 depend A7 ; A7 depend A3;
 * A1.a=101 A2.a=102 A3.a=103 ... A9.a=109
 * RC-Testing_Result=(101+102)+(102+103)+(103+101+105+106)+(104+103)+(105+104)+(106+107)+(107+103)=1662
 *
 */

class Cycle_a_00210_A1 {
    Cycle_a_00210_A2 a2_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_00210_A1(String strObjectName) {
        a2_0 = null;
        a = 101;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
    }

    void add() {
        sum = a + a2_0.a;
    }
}

class Cycle_a_00210_A2 {
    Cycle_a_00210_A3 a3_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_00210_A2(String strObjectName) {
        a3_0 = null;
        a = 102;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
    }

    void add() {
        sum = a + a3_0.a;
    }
}

class Cycle_a_00210_A3 {
    Cycle_a_00210_A1 a1_0;
    Cycle_a_00210_A5 a5_0;
    Cycle_a_00210_A6 a6_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_00210_A3(String strObjectName) {
        a1_0 = null;
        a5_0 = null;
        a6_0 = null;
        a = 103;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
    }

    void add() {
        sum = a + a1_0.a + a5_0.a + a6_0.a;
    }
}

class Cycle_a_00210_A4 {
    Cycle_a_00210_A3 a3_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_00210_A4(String strObjectName) {
        a3_0 = null;
        a = 104;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
    }

    void add() {
        sum = a + a3_0.a;
    }
}

class Cycle_a_00210_A5 {
    Cycle_a_00210_A4 a4_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_00210_A5(String strObjectName) {
        a4_0 = null;
        a = 105;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
    }

    void add() {
        sum = a + a4_0.a;
    }
}

class Cycle_a_00210_A6 {
    Cycle_a_00210_A7 a7_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_00210_A6(String strObjectName) {
        a7_0 = null;
        a = 106;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
    }

    void add() {
        sum = a + a7_0.a;
    }
}

class Cycle_a_00210_A7 {
    Cycle_a_00210_A3 a3_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_00210_A7(String strObjectName) {
        a3_0 = null;
        a = 107;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
    }

    void add() {
        sum = a + a3_0.a;
    }
}


public class Cycle_a_00210 {

    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();

    }

    private static void rc_testcase_main_wrapper() {
        Cycle_a_00210_A1 a1_main = new Cycle_a_00210_A1("a1_main");
        Cycle_a_00210_A4 a4_main = new Cycle_a_00210_A4("a4_main");
        a1_main.a2_0 = new Cycle_a_00210_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_00210_A3("a3_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a5_0 = new Cycle_a_00210_A5("a5_0");
        a1_main.a2_0.a3_0.a5_0.a4_0 = a4_main;
        a4_main.a3_0 = a1_main.a2_0.a3_0;
        a1_main.a2_0.a3_0.a6_0 = new Cycle_a_00210_A6("a6_0");
        a1_main.a2_0.a3_0.a6_0.a7_0 = new Cycle_a_00210_A7("a7_0");
        a1_main.a2_0.a3_0.a6_0.a7_0.a3_0 = a1_main.a2_0.a3_0;

        a1_main.add();
        a4_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a5_0.add();
        a1_main.a2_0.a3_0.a6_0.add();
        a1_main.a2_0.a3_0.a6_0.a7_0.add();

        int result = a1_main.sum + a4_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a5_0.sum + a1_main.a2_0.a3_0.a6_0.sum + a1_main.a2_0.a3_0.a6_0.a7_0.sum;
        if (result == 1662)
            System.out.println("ExpectResult");
    }

}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\nExpectResult\n