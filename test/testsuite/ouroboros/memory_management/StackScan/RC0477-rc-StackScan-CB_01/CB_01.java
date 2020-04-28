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
 * -@TestCaseID: Maple_MemoryManagement2.0_CB01
 *- @TestCaseName: CB_01
 *- @TestCaseType: Function Testing for placementRCTest
 *- @RequirementName: 运行时支持GCOnly
 *- @Brief:一组有环的对象，他们的field指向一个hashMap，里面装满了对象
 *  -#step1: 创建一个环，环的类型参考了Cycle_a_0038.java;
 *  -#step2: 两个域test1和test2,里面装满了对象，对象格式参考Nocycle_00180.java，这样就构造了一个大对象。
 *  -#step3: 验证结果正确，再用GCverify验证无内存泄漏。
 *- @Expect:ExpectResult\n
 *- @Priority: High
 *- @Source: CB_01.java
 *- @ExecuteClass: CB_01
 *- @ExecuteArgs:
 */

import java.util.HashMap;

class CB_Nolcycle_a_00180_A1 {
    CB_Nolcycle_a_00180_B1 b1_0;
    CB_Nolcycle_a_00180_D1 d1_0;
    int a;
    int sum;
    String strObjectName;

    CB_Nolcycle_a_00180_A1(String strObjectName) {
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


class CB_Nolcycle_a_00180_A2 {
    CB_Nolcycle_a_00180_B2 b2_0;
    int a;
    int sum;
    String strObjectName;

    CB_Nolcycle_a_00180_A2(String strObjectName) {
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


class CB_Nolcycle_a_00180_A3 {
    CB_Nolcycle_a_00180_B2 b2_0;
    CB_Nolcycle_a_00180_C2 c2_0;
    int a;
    int sum;
    String strObjectName;

    CB_Nolcycle_a_00180_A3(String strObjectName) {
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

class CB_Nolcycle_a_00180_A4 {
    CB_Nolcycle_a_00180_B3 b3_0;
    CB_Nolcycle_a_00180_C2 c2_0;
    int a;
    int sum;
    String strObjectName;

    CB_Nolcycle_a_00180_A4(String strObjectName) {
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


class CB_Nolcycle_a_00180_B1 {
    CB_Nolcycle_a_00180_D2 d2_0;
    int a;
    int sum;
    String strObjectName;

    CB_Nolcycle_a_00180_B1(String strObjectName) {
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


class CB_Nolcycle_a_00180_B2 {
    CB_Nolcycle_a_00180_C1 c1_0;
    CB_Nolcycle_a_00180_D1 d1_0;
    CB_Nolcycle_a_00180_D2 d2_0;
    CB_Nolcycle_a_00180_D3 d3_0;
    int a;
    int sum;
    String strObjectName;

    CB_Nolcycle_a_00180_B2(String strObjectName) {
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


class CB_Nolcycle_a_00180_B3 {
    CB_Nolcycle_a_00180_C1 c1_0;
    int a;
    int sum;
    String strObjectName;

    CB_Nolcycle_a_00180_B3(String strObjectName) {
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


class CB_Nolcycle_a_00180_C1 {
    CB_Nolcycle_a_00180_D1 d1_0;
    int a;
    int sum;
    String strObjectName;

    CB_Nolcycle_a_00180_C1(String strObjectName) {
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

class CB_Nolcycle_a_00180_C2 {
    CB_Nolcycle_a_00180_D2 d2_0;
    CB_Nolcycle_a_00180_D3 d3_0;
    int a;
    int sum;
    String strObjectName;

    CB_Nolcycle_a_00180_C2(String strObjectName) {
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


class CB_Nolcycle_a_00180_D1 {
    int a;
    int sum;
    String strObjectName;

    CB_Nolcycle_a_00180_D1(String strObjectName) {
        a = 401;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D1_"+strObjectName);
    }

    void add() {
        sum = a + a;
    }
}

class CB_Nolcycle_a_00180_D2 {
    int a;
    int sum;
    String strObjectName;

    CB_Nolcycle_a_00180_D2(String strObjectName) {
        a = 402;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D2_"+strObjectName);
    }

    void add() {
        sum = a + a;
    }
}

//该树有12个节点
class CB_Nolcycle_a_00180_D3 {
    int a;
    int sum;
    String strObjectName;

    CB_Nolcycle_a_00180_D3(String strObjectName) {
        a = 403;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D3_"+strObjectName);
    }

    void add() {
        sum = a + a;
    }
}

class CB_Nolcycle_a_00180 {
    public CB_Nolcycle_a_00180() {
        CB_Nolcycle_a_00180_A1 a1_main = new CB_Nolcycle_a_00180_A1("a1_main");
        CB_Nolcycle_a_00180_A2 a2_main = new CB_Nolcycle_a_00180_A2("a2_main");
        CB_Nolcycle_a_00180_A3 a3_main = new CB_Nolcycle_a_00180_A3("a3_main");
        CB_Nolcycle_a_00180_A4 a4_main = new CB_Nolcycle_a_00180_A4("a4_main");
        a1_main.b1_0 = new CB_Nolcycle_a_00180_B1("b1_0");
        a1_main.d1_0 = new CB_Nolcycle_a_00180_D1("d1_0");
        a1_main.b1_0.d2_0 = new CB_Nolcycle_a_00180_D2("d2_0");

        a2_main.b2_0 = new CB_Nolcycle_a_00180_B2("b2_0");
        a2_main.b2_0.c1_0 = new CB_Nolcycle_a_00180_C1("c1_0");
        a2_main.b2_0.d1_0 = new CB_Nolcycle_a_00180_D1("d1_0");
        a2_main.b2_0.d2_0 = new CB_Nolcycle_a_00180_D2("d2_0");
        a2_main.b2_0.d3_0 = new CB_Nolcycle_a_00180_D3("d3_0");
        a2_main.b2_0.c1_0.d1_0 = new CB_Nolcycle_a_00180_D1("d1_0");

        a3_main.b2_0 = new CB_Nolcycle_a_00180_B2("b2_0");
        a3_main.b2_0.c1_0 = new CB_Nolcycle_a_00180_C1("c1_0");
        a3_main.b2_0.c1_0.d1_0 = new CB_Nolcycle_a_00180_D1("d1_0");
        a3_main.b2_0.d1_0 = new CB_Nolcycle_a_00180_D1("d1_0");
        a3_main.b2_0.d2_0 = new CB_Nolcycle_a_00180_D2("d2_0");
        a3_main.b2_0.d3_0 = new CB_Nolcycle_a_00180_D3("d3_0");

        a3_main.c2_0 = new CB_Nolcycle_a_00180_C2("c2_0");
        a3_main.c2_0.d2_0 = new CB_Nolcycle_a_00180_D2("d2_0");
        a3_main.c2_0.d3_0 = new CB_Nolcycle_a_00180_D3("d3_0");

        a4_main.b3_0 = new CB_Nolcycle_a_00180_B3("b3_0");
        a4_main.b3_0.c1_0 = new CB_Nolcycle_a_00180_C1("c1_0");
        a4_main.b3_0.c1_0.d1_0 = new CB_Nolcycle_a_00180_D1("d1_0");
        a4_main.c2_0 = new CB_Nolcycle_a_00180_C2("c2_0");
        a4_main.c2_0.d2_0 = new CB_Nolcycle_a_00180_D2("d2_0");
        a4_main.c2_0.d3_0 = new CB_Nolcycle_a_00180_D3("d3_0");
    }
}

class CB_01_A1 {
    static HashMap test1;
    static
    int a;
    CB_01_A2 a2_0;
    CB_01_A3 a3_0;
    int sum;
    String strObjectName;

    CB_01_A1(String strObjectName) {
        a2_0 = null;
        a3_0 = null;
        a = 101;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
    }

    void add() {
        sum = a + a2_0.a + a3_0.a;
    }

    @Override
    public void finalize() throws Throwable {
        super.finalize();
        //System.out.println("A finalize");

        CB_01.check = this;

    }
}

class CB_01_A2 {
    static CB_01_A1 a1_0;
    volatile static HashMap test2;
    CB_01_A3 a3_0;
    int a;
    int sum;
    String strObjectName;

    CB_01_A2(String strObjectName) {
        a1_0 = null;
        a3_0 = null;
        a = 102;
        sum = 0;
        this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A2_"+strObjectName);
    }

    void add() {
        sum = a + CB_01_A1.a + a3_0.a;
    }

    @Override
    public void finalize() throws Throwable {
        super.finalize();
    }
}

class CB_01_A3 {
    static CB_Nolcycle_a_00180[] test1;
    CB_01_A1 a1_0;
    CB_01_A2 a2_0;
    int a;
    int sum;
    String strObjectName;

    CB_01_A3(String strObjectName) {
        a1_0 = null;
        a2_0 = null;
        a = 103;
        sum = 0;
        this.strObjectName = strObjectName;
    }

    void add() {
        sum = a + CB_01_A1.a + a2_0.a;
    }
}


public class CB_01 {
    public static CB_01_A1 check = null;
    private static CB_01_A1 a1_main = null;

    private CB_01() {
        a1_main = new CB_01_A1("a1_main");
        a1_main.a2_0 = new CB_01_A2("a2_0");
        CB_01_A2.a1_0 = a1_main;
        a1_main.a2_0.a3_0 = new CB_01_A3("a3_0");
        a1_main.a3_0 = a1_main.a2_0.a3_0;
        a1_main.a3_0.a1_0 = a1_main;
        a1_main.a3_0.a2_0 = a1_main.a2_0;

        a1_main.add();
        a1_main.a2_0.add();
        a1_main.a2_0.a3_0.add();
    }

    private static void test_CB_01(int times) {
        CB_01_A1.test1 = new HashMap();
        CB_01_A2.test2 = new HashMap();
        for (int i = 0; i < times; i++) {
            CB_01_A1.test1.put(i, new CB_Nolcycle_a_00180());
            CB_01_A2.test2.put(i, new CB_Nolcycle_a_00180());
        }
    }

    private static void rc_testcase_main_wrapper() {
        CB_01 cb01 = new CB_01();
        test_CB_01(10000);
        check = a1_main;
        try {
            int result = CB_01.check.sum + CB_01.check.a2_0.sum + CB_01.check.a2_0.a3_0.sum + CB_01_A1.test1.size() + CB_01_A2.test2.size();
            if (result == 20918)
                System.out.println("ExpectResult");
        } catch (NullPointerException n) {
            System.out.println("ErrorResult");
        }
    }

    public static void main(String[] args) {
        rc_testcase_main_wrapper();
    }

}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan ExpectResult\n