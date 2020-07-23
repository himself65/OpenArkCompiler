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
 * -@TestCaseID:maple/runtime/rc/function/RC_Dec/Nocycle_aDec_00030.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: Nocycle_aDec_00030 in RC测试-No-Cycle-00.vsd.
 *- @Brief:functionTest
 *- @Expect:ExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: Nocycle_aDec_00030.java
 *- @ExecuteClass: Nocycle_aDec_00030
 *- @ExecuteArgs:
 */

class Nocycle_aDec_00030_A1 {
    Nocycle_aDec_00030_B1 b1_0;
    Nocycle_aDec_00030_B2 b2_0;
    Nocycle_aDec_00030_B3 b3_0;
    Nocycle_aDec_00030_B4 b4_0;
    int a;
    int sum;
    String strObjectName;

    Nocycle_aDec_00030_A1(String strObjectName) {
        b1_0 = null;
        b2_0 = null;
        b3_0 = null;
        b4_0 = null;
        a = 101;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
    }

    void add() {
        sum = a + b1_0.a + b2_0.a + b3_0.a + b4_0.a;
    }
}

class Nocycle_aDec_00030_B1 {
    Nocycle_aDec_00030_C1 c1_0;
    Nocycle_aDec_00030_C2 c2_0;
    Nocycle_aDec_00030_C3 c3_0;
    int a;
    int sum;
    String strObjectName;

    Nocycle_aDec_00030_B1(String strObjectName) {
        c1_0 = null;
        c2_0 = null;
        c3_0 = null;
        a = 201;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
    }

    void add() {
        sum = a + c1_0.a + c2_0.a;
    }
}

class Nocycle_aDec_00030_B2 {
    int a;
    int sum;
    String strObjectName;

    Nocycle_aDec_00030_B2(String strObjectName) {
        a = 202;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B2_"+strObjectName);
    }

    void add() {
        sum = a + a;

    }
}


class Nocycle_aDec_00030_B3 {
    int a;
    int sum;
    String strObjectName;

    Nocycle_aDec_00030_B3(String strObjectName) {
        a = 203;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B3_"+strObjectName);
    }

    void add() {
        sum = a + a;

    }
}

class Nocycle_aDec_00030_B4 {
    int a;
    int sum;
    String strObjectName;

    Nocycle_aDec_00030_B4(String strObjectName) {
        a = 204;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B4_"+strObjectName);
    }

    void add() {
        sum = a + a;

    }
}

class Nocycle_aDec_00030_C1 {
    int a;
    int sum;
    String strObjectName;

    Nocycle_aDec_00030_C1(String strObjectName) {
        a = 301;
        sum = 0;
        this.strObjectName = strObjectName;
    }

    void add() {
        sum = a + a;
    }
}

class Nocycle_aDec_00030_C2 {
    int a;
    int sum;
    String strObjectName;

    Nocycle_aDec_00030_C2(String strObjectName) {
        a = 302;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C2_"+strObjectName);
    }

    void add() {
        sum = a + a;
    }
}

class Nocycle_aDec_00030_C3 {
    int a;
    int sum;
    String strObjectName;

    Nocycle_aDec_00030_C3(String strObjectName) {
        a = 303;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C3_"+strObjectName);
    }

    void add() {
        sum = a + a;
    }
}

public class Nocycle_aDec_00030 {

    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();

    }

    private static void rc_testcase_main_wrapper() {
        Nocycle_aDec_00030_A1 a1_main = new Nocycle_aDec_00030_A1("a1_main");
        a1_main.b1_0 = new Nocycle_aDec_00030_B1("b1_0");
        a1_main.b1_0.c1_0 = new Nocycle_aDec_00030_C1("c1_0");
        a1_main.b1_0.c2_0 = new Nocycle_aDec_00030_C2("c2_0");
        a1_main.b1_0.c3_0 = new Nocycle_aDec_00030_C3("c3_0");
        a1_main.b2_0 = new Nocycle_aDec_00030_B2("b2_0");
        a1_main.b3_0 = new Nocycle_aDec_00030_B3("b3_0");
        a1_main.b4_0 = new Nocycle_aDec_00030_B4("b4_0");
        modifyC3(a1_main.b1_0.c3_0);
        a1_main.b1_0.c3_0 = null;
        a1_main.add();
        a1_main.b1_0.add();
        a1_main.b2_0.add();
        a1_main.b3_0.add();
        a1_main.b4_0.add();
        a1_main.b1_0.c1_0.add();
        a1_main.b1_0.c2_0.add();

        int result = a1_main.sum + a1_main.b1_0.sum + a1_main.b2_0.sum + a1_main.b3_0.sum + a1_main.b4_0.sum + a1_main.b1_0.c1_0.sum + a1_main.b1_0.c2_0.sum;
        if (result == 4139)
            System.out.println("ExpectResult");
    }

    public static void modifyC3(Nocycle_aDec_00030_C3 c3) {
        c3.a += 1;
    }

}


// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\nExpectResult\n