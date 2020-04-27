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
 * -@TestCaseID:maple/runtime/rc/function/RC_Dec/Nocycle_aDec_00060.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: Nocycle_aDec_00060 in RC测试-No-Cycle-00.vsd.
 *- @Brief:functionTest
 *- @Expect:ExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: Nocycle_aDec_00060.java
 *- @ExecuteClass: Nocycle_aDec_00060
 *- @ExecuteArgs:
 */


class Nocycle_aDec_00060_A1 {
    Nocycle_aDec_00060_B1 b1_0;
    Nocycle_aDec_00060_D1 d1_0;
    int a;
    int sum;
    String strObjectName;

    Nocycle_aDec_00060_A1(String strObjectName) {
        b1_0 = null;
        d1_0 = null;
        a = 101;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
    }

    void add() {
        sum = a + b1_0.a + d1_0.a;
    }
}


class Nocycle_aDec_00060_A2 {
    Nocycle_aDec_00060_B2 b2_0;
    int a;
    int sum;
    String strObjectName;

    Nocycle_aDec_00060_A2(String strObjectName) {
        b2_0 = null;
        a = 102;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
    }

    void add() {
        sum = a + b2_0.a;
    }
}


class Nocycle_aDec_00060_A3 {
    Nocycle_aDec_00060_B2 b2_0;
    Nocycle_aDec_00060_C2 c2_0;
    int a;
    int sum;
    String strObjectName;

    Nocycle_aDec_00060_A3(String strObjectName) {
        b2_0 = null;
        c2_0 = null;
        a = 103;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
    }

    void add() {
        sum = a + b2_0.a + c2_0.a;
    }
}

class Nocycle_aDec_00060_A4 {
    Nocycle_aDec_00060_B3 b3_0;
    Nocycle_aDec_00060_C2 c2_0;
    int a;
    int sum;
    String strObjectName;

    Nocycle_aDec_00060_A4(String strObjectName) {
        b3_0 = null;
        c2_0 = null;
        a = 104;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
    }

    void add() {
        sum = a + b3_0.a + c2_0.a;
    }
}


class Nocycle_aDec_00060_B1 {
    Nocycle_aDec_00060_D2 d2_0;
    int a;
    int sum;
    String strObjectName;

    Nocycle_aDec_00060_B1(String strObjectName) {
        d2_0 = null;
        a = 201;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
    }

    void add() {
        sum = a + d2_0.a;
    }
}


class Nocycle_aDec_00060_B2 {
    Nocycle_aDec_00060_C1 c1_0;
    Nocycle_aDec_00060_D1 d1_0;
    Nocycle_aDec_00060_D2 d2_0;
    Nocycle_aDec_00060_D3 d3_0;
    int a;
    int sum;
    String strObjectName;

    Nocycle_aDec_00060_B2(String strObjectName) {
        c1_0 = null;
        d1_0 = null;
        d2_0 = null;
        d3_0 = null;
        a = 202;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B2_"+strObjectName);
    }

    void add() {
        sum = a + c1_0.a + d1_0.a + d2_0.a + d3_0.a;
    }
}


class Nocycle_aDec_00060_B3 {
    Nocycle_aDec_00060_C1 c1_0;
    int a;
    int sum;
    String strObjectName;

    Nocycle_aDec_00060_B3(String strObjectName) {
        c1_0 = null;
        a = 203;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B3_"+strObjectName);
    }

    void add() {
        sum = a + c1_0.a;
    }
}


class Nocycle_aDec_00060_C1 {
    Nocycle_aDec_00060_D1 d1_0;
    int a;
    int sum;
    String strObjectName;

    Nocycle_aDec_00060_C1(String strObjectName) {
        d1_0 = null;
        a = 301;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C1_"+strObjectName);
    }

    void add() {
        sum = a + d1_0.a;
    }
}

class Nocycle_aDec_00060_C2 {
    Nocycle_aDec_00060_D2 d2_0;
    Nocycle_aDec_00060_D3 d3_0;
    int a;
    int sum;
    String strObjectName;

    Nocycle_aDec_00060_C2(String strObjectName) {
        d2_0 = null;
        d3_0 = null;
        a = 302;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C2_"+strObjectName);
    }

    void add() {
        sum = a + d2_0.a + d3_0.a;
    }
}


class Nocycle_aDec_00060_D1 {
    int a;
    int sum;
    String strObjectName;

    Nocycle_aDec_00060_D1(String strObjectName) {
        a = 401;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D1_"+strObjectName);
    }

    void add() {
        sum = a + a;
    }
}

class Nocycle_aDec_00060_D2 {
    int a;
    int sum;
    String strObjectName;

    Nocycle_aDec_00060_D2(String strObjectName) {
        a = 402;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D2_"+strObjectName);
    }

    void add() {
        sum = a + a;
    }
}

class Nocycle_aDec_00060_D3 {
    int a;
    int sum;
    String strObjectName;

    Nocycle_aDec_00060_D3(String strObjectName) {
        a = 403;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D3_"+strObjectName);
    }

    void add() {
        sum = a + a;
    }
}


public class Nocycle_aDec_00060 {

    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();

    }

    private static void rc_testcase_main_wrapper() {
        Nocycle_aDec_00060_A1 a1_main = new Nocycle_aDec_00060_A1("a1_main");
        Nocycle_aDec_00060_A2 a2_main = new Nocycle_aDec_00060_A2("a2_main");
        Nocycle_aDec_00060_A3 a3_main = new Nocycle_aDec_00060_A3("a3_main");
        Nocycle_aDec_00060_A4 a4_main = new Nocycle_aDec_00060_A4("a4_main");
        a1_main.b1_0 = new Nocycle_aDec_00060_B1("b1_0");
        a1_main.d1_0 = new Nocycle_aDec_00060_D1("d1_0");
        a1_main.b1_0.d2_0 = new Nocycle_aDec_00060_D2("d2_0");

        a2_main.b2_0 = new Nocycle_aDec_00060_B2("b2_0");
        a2_main.b2_0.c1_0 = new Nocycle_aDec_00060_C1("c1_0");
        a2_main.b2_0.d1_0 = a1_main.d1_0;
        a2_main.b2_0.d2_0 = new Nocycle_aDec_00060_D2("d2_0");
        a2_main.b2_0.d3_0 = new Nocycle_aDec_00060_D3("d3_0");
        a2_main.b2_0.c1_0.d1_0 = a1_main.d1_0;

        a3_main.b2_0 = a2_main.b2_0;
        a3_main.b2_0.c1_0 = a2_main.b2_0.c1_0;
        a3_main.b2_0.c1_0.d1_0 = a2_main.b2_0.c1_0.d1_0;
        a3_main.b2_0.d1_0 = a2_main.b2_0.d1_0;
        a3_main.b2_0.d2_0 = a2_main.b2_0.d2_0;
        a3_main.b2_0.d3_0 = a2_main.b2_0.d3_0;

        a3_main.c2_0 = new Nocycle_aDec_00060_C2("c2_0");
        a3_main.c2_0.d2_0 = a2_main.b2_0.d2_0;
        a3_main.c2_0.d3_0 = new Nocycle_aDec_00060_D3("d3_0");

        a4_main.b3_0 = new Nocycle_aDec_00060_B3("b3_0");
        a4_main.b3_0.c1_0 = a3_main.b2_0.c1_0;
        a4_main.b3_0.c1_0.d1_0 = a3_main.b2_0.c1_0.d1_0;
        a4_main.c2_0 = a3_main.c2_0;
        a4_main.c2_0.d2_0 = a3_main.c2_0.d2_0;
        a4_main.c2_0.d3_0 = a3_main.c2_0.d3_0;
        ModifyA1(a1_main);
        ModifyA2(a2_main);
        ModifyA3(a3_main);
        a4_main.add();
        a4_main.b3_0.add();
        a4_main.b3_0.c1_0.add();
        a4_main.b3_0.c1_0.d1_0.add();
        a4_main.c2_0.add();
        a4_main.c2_0.d2_0.add();
        a4_main.c2_0.d3_0.add();


        int result = a4_main.sum + a4_main.b3_0.sum + a4_main.b3_0.c1_0.sum + a4_main.c2_0.sum + a4_main.b3_0.c1_0.d1_0.sum + a4_main.c2_0.d2_0.sum + a4_main.c2_0.d3_0.sum;

        if (result == 5334)
            System.out.println("ExpectResult");
    }

    private static void ModifyA1(Nocycle_aDec_00060_A1 a1) {
        a1.a += 1;
        a1.add();
        a1.b1_0.add();
        a1.d1_0.add();
        a1.b1_0.d2_0.add();
    }

    private static void ModifyA2(Nocycle_aDec_00060_A2 a2) {
        a2.a += 1;
        a2.add();
        a2.b2_0.add();
        a2.b2_0.c1_0.add();
        a2.b2_0.d1_0.add();
        a2.b2_0.d2_0.add();
        a2.b2_0.d3_0.add();
        a2.b2_0.c1_0.d1_0.add();
    }

    private static void ModifyA3(Nocycle_aDec_00060_A3 a3) {
        a3.a += 1;
        a3.add();
        a3.b2_0.add();
        a3.b2_0.c1_0.add();
        a3.b2_0.c1_0.d1_0.add();
        a3.b2_0.d1_0.add();
        a3.b2_0.d2_0.add();
        a3.b2_0.d3_0.add();
        a3.c2_0.add();
        a3.c2_0.d2_0.add();
        a3.c2_0.d3_0.add();
    }

}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\nExpectResult\n