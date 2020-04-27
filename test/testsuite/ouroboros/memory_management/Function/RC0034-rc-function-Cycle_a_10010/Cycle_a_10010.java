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
 * -@TestCaseID:maple/runtime/rc/function/Cycle_a_10010.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination: Cycle_a_10010 in RC测试-Cycle-01
 *- @Brief:functionTest
 *- @Expect:ExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: Cycle_a_10010.java
 *- @ExecuteClass: Cycle_a_10010
 *- @ExecuteArgs:
 */
class Cycle_a_10010_A1 {
    Cycle_a_10010_A2 a2_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_10010_A1(String strObjectName) {
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

class Cycle_a_10010_A2 {
    Cycle_a_10010_A3 a3_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_10010_A2(String strObjectName) {
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

class Cycle_a_10010_A3 {
    Cycle_a_10010_A1 a1_0;
    Cycle_a_10010_A5 a5_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_10010_A3(String strObjectName) {
        a1_0 = null;
        a5_0 = null;
        a = 103;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
    }

    void add() {
        sum = a + a1_0.a + a5_0.a;
    }
}

class Cycle_a_10010_A4 {
    Cycle_a_10010_A3 a3_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_10010_A4(String strObjectName) {
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

class Cycle_a_10010_A5 {
    Cycle_a_10010_A4 a4_0;
    Cycle_a_10010_A7 a7_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_10010_A5(String strObjectName) {
        a4_0 = null;
        a7_0 = null;
        a = 105;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
    }

    void add() {
        sum = a + a4_0.a + a7_0.a;
    }
}

class Cycle_a_10010_A6 {
    Cycle_a_10010_A5 a5_0;
    Cycle_a_10010_A7 a7_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_10010_A6(String strObjectName) {
        a5_0 = null;
        a7_0 = null;
        a = 106;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
    }

    void add() {
        sum = a + a5_0.a + a7_0.a;
    }
}

class Cycle_a_10010_A7 {
    int a;
    int sum;
    String strObjectName;

    Cycle_a_10010_A7(String strObjectName) {
        a = 107;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
    }

    void add() {
        sum = a + a;
    }
}

class Cycle_a_10010_2A1 {
    Cycle_a_10010_2A2 a2_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_10010_2A1(String strObjectName) {
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

class Cycle_a_10010_2A2 {
    Cycle_a_10010_2A3 a3_0;
    Cycle_a_10010_2A4 a4_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_10010_2A2(String strObjectName) {
        a3_0 = null;
        a4_0 = null;
        a = 102;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
    }

    void add() {
        sum = a + a3_0.a + a4_0.a;
    }
}

class Cycle_a_10010_2A3 {
    Cycle_a_10010_2A1 a1_0;
    Cycle_a_10010_2A5 a5_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_10010_2A3(String strObjectName) {
        a1_0 = null;
        a5_0 = null;
        a = 103;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A3_"+strObjectName);
    }

    void add() {
        sum = a + a1_0.a + a5_0.a;
    }
}


class Cycle_a_10010_2A4 {
    Cycle_a_10010_2A1 a1_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_10010_2A4(String strObjectName) {
        a1_0 = null;
        a = 104;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A4_"+strObjectName);
    }

    void add() {
        sum = a + a1_0.a;
    }
}

class Cycle_a_10010_2A5 {
    Cycle_a_10010_2A6 a6_0;
    Cycle_a_10010_2A8 a8_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_10010_2A5(String strObjectName) {
        a6_0 = null;
        a8_0 = null;
        a = 105;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A5_"+strObjectName);
    }

    void add() {
        sum = a + a6_0.a + a8_0.a;
    }
}

class Cycle_a_10010_2A6 {
    Cycle_a_10010_2A1 a1_0;
    Cycle_a_10010_2A3 a3_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_10010_2A6(String strObjectName) {
        a1_0 = null;
        a3_0 = null;
        a = 106;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A6_"+strObjectName);
    }

    void add() {
        sum = a + a1_0.a + a3_0.a;
    }
}


class Cycle_a_10010_2A7 {
    Cycle_a_10010_2A3 a3_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_10010_2A7(String strObjectName) {
        a3_0 = null;
        a = 107;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A7_"+strObjectName);
    }

    void add() {
        sum = a + a3_0.a;
    }
}


class Cycle_a_10010_2A8 {
    Cycle_a_10010_2A7 a7_0;
    int a;
    int sum;
    String strObjectName;

    Cycle_a_10010_2A8(String strObjectName) {
        a7_0 = null;
        a = 108;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A8_"+strObjectName);
    }

    void add() {
        sum = a + a7_0.a;
    }
}


public class Cycle_a_10010 {
    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();

    }

    private static void rc_testcase_main_wrapper() {
        Cycle_a_10010_A1 a1_main = new Cycle_a_10010_A1("a1_main");
        Cycle_a_10010_A4 a4_main = new Cycle_a_10010_A4("a4_main");
        Cycle_a_10010_A6 a6_main = new Cycle_a_10010_A6("a6_main");
        a1_main.a2_0 = new Cycle_a_10010_A2("a2_0");
        a1_main.a2_0.a3_0 = new Cycle_a_10010_A3("a3_0");
        a1_main.a2_0.a3_0.a1_0 = a1_main;
        a1_main.a2_0.a3_0.a5_0 = new Cycle_a_10010_A5("a5_0");
        a1_main.a2_0.a3_0.a5_0.a4_0 = a4_main;
        a4_main.a3_0 = a1_main.a2_0.a3_0;
        a1_main.a2_0.a3_0.a5_0.a7_0 = new Cycle_a_10010_A7("a7_0");
        a6_main.a7_0 = a1_main.a2_0.a3_0.a5_0.a7_0;
        a6_main.a5_0 = a1_main.a2_0.a3_0.a5_0;
        a1_main.add();
        a4_main.add();
        a6_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
        a1_main.a2_0.a3_0.a5_0.add();
        a1_main.a2_0.a3_0.a5_0.a7_0.add();
        int result = a1_main.sum + a4_main.sum + a6_main.sum + a1_main.a2_0.sum + a1_main.a2_0.a3_0.sum + a1_main.a2_0.a3_0.a5_0.sum + a6_main.a7_0.sum;
        Cycle_a_10010_2A1 a1_2main = new Cycle_a_10010_2A1("a1_2main");
        Cycle_a_10010_2A4 a4_2main = new Cycle_a_10010_2A4("a4_2main");
        Cycle_a_10010_2A6 a6_2main = new Cycle_a_10010_2A6("a6_2main");
        a1_2main.a2_0 = new Cycle_a_10010_2A2("a2_0");
        a1_2main.a2_0.a3_0 = new Cycle_a_10010_2A3("a3_0");
        a1_2main.a2_0.a4_0 = a4_2main;
        a4_2main.a1_0 = a1_2main;
        a1_2main.a2_0.a3_0.a1_0 = a1_2main;
        a1_2main.a2_0.a3_0.a5_0 = new Cycle_a_10010_2A5("a5_0");
        a1_2main.a2_0.a3_0.a5_0.a6_0 = a6_2main;
        a1_2main.a2_0.a3_0.a5_0.a8_0 = new Cycle_a_10010_2A8("a8_0");
        a1_2main.a2_0.a3_0.a5_0.a8_0.a7_0 = new Cycle_a_10010_2A7("a7_0");
        a1_2main.a2_0.a3_0.a5_0.a8_0.a7_0.a3_0 = a1_2main.a2_0.a3_0;
        a6_2main.a1_0 = a1_2main;
        a6_2main.a3_0 = a1_2main.a2_0.a3_0;
        a1_2main.add();
        a4_2main.add();
        a6_2main.add();
        a1_2main.a2_0.add();
        a1_2main.a2_0.a3_0.add();
        a1_2main.a2_0.a3_0.a5_0.add();
        a1_2main.a2_0.a3_0.a5_0.a8_0.add();
        a1_2main.a2_0.a3_0.a5_0.a8_0.a7_0.add();
        int result2 = a1_2main.sum + a4_2main.sum + a6_2main.sum + a1_2main.a2_0.sum + a1_2main.a2_0.a3_0.sum + a1_2main.a2_0.a3_0.a5_0.sum + a6_2main.a3_0.a5_0.a8_0.a7_0.sum + a6_2main.a3_0.a5_0.a8_0.sum;
        result += result2;
        if (result == 3852)
            System.out.println("ExpectResult");
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\nExpectResult\n