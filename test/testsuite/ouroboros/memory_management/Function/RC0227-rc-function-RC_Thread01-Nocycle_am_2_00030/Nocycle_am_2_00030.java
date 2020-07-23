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
 * -@TestCaseID:maple/runtime/rc/function/RC_Thread01/Nocycle_am_2_00030.java
 *- @TestCaseName:MyselfClassName
 *- @RequirementName:[运行时需求]支持自动内存管理
 *- @Title/Destination:Change Nocycle_a_2_00030 in RC测试-No-Cycle-00.vsd to Multi thread testcase.
 *- @Brief:functionTest
 *- @Expect:ExpectResult\nExpectResult\n
 *- @Priority: High
 *- @Source: Nocycle_am_2_00030.java
 *- @ExecuteClass: Nocycle_am_2_00030
 *- @ExecuteArgs:
 */
class ThreadRc_Nocycle_am_2_00010B extends Thread {
    private boolean checkout;

    public void run() {
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
        //         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a1_main.b1_0.sum+a1_main.b2_0.sum+a1_main.b3_0.sum);
        int result = a1_main.sum + a1_main.b1_0.sum + a1_main.b2_0.sum + a1_main.b3_0.sum + a2_main.sum;
        //System.out.println("RC-Testing_Result="+result);

        if (result == 1939)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

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
}

class ThreadRc_Nocycle_am_2_00020B extends Thread {
    private boolean checkout;

    public void run() {
        Nocycle_a_2_00020_A1 a1_main = new Nocycle_a_2_00020_A1("a1_main");
        a1_main.b1_0 = new Nocycle_a_2_00020_B1("b1_0");
        a1_main.b2_0 = new Nocycle_a_2_00020_B2("b2_0");
        a1_main.b3_0 = new Nocycle_a_2_00020_B3("b3_0");
        a1_main.b4_0 = new Nocycle_a_2_00020_B4("b4_0");
        a1_main.b4_0.c1_0 = new Nocycle_a_2_00020_C1("c1_0");
        a1_main.b4_0.c2_0 = new Nocycle_a_2_00020_C2("c2_0");
        a1_main.b4_0.c3_0 = new Nocycle_a_2_00020_C3("c3_0");
        a1_main.add();
        a1_main.b1_0.add();
        a1_main.b2_0.add();
        a1_main.b3_0.add();
        a1_main.b4_0.add();
        a1_main.b4_0.c1_0.add();
        a1_main.b4_0.c2_0.add();
        a1_main.b4_0.c3_0.add();
//         System.out.printf("RC-Testing_Result=%d\n",a1_main.sum+a1_main.b1_0.sum+a1_main.b2_0.sum+a1_main.b3_0.sum+a1_main.b4_0.sum+a1_main.b4_0.c1_0.sum+a1_main.b4_0.c2_0.sum+a1_main.b4_0.c3_0.sum);

        int result1 = a1_main.sum + a1_main.b1_0.sum + a1_main.b2_0.sum + a1_main.b3_0.sum + a1_main.b4_0.sum + a1_main.b4_0.c1_0.sum + a1_main.b4_0.c2_0.sum + a1_main.b4_0.c3_0.sum;
        //System.out.println("RC-Testing_Result="+result1);
        Nocycle_a_2_00020_2A1 a1_main2 = new Nocycle_a_2_00020_2A1("a1_main2");
        a1_main2.b1_0 = new Nocycle_a_2_00020_2B1("b1_0");
        a1_main2.b1_0.c1_0 = new Nocycle_a_2_00020_2C1("c1_0");
        a1_main2.b1_0.c1_0.d1_0 = new Nocycle_a_2_00020_2D1("d1_0");
        a1_main2.b1_0.c1_0.d1_0.e1_0 = new Nocycle_a_2_00020_2E1("e1_0");
        a1_main2.b1_0.c1_0.d1_0.e1_0.f1_0 = new Nocycle_a_2_00020_2F1("f1_0");
        a1_main2.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0 = new Nocycle_a_2_00020_2G1("g1_0");
        a1_main2.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0 = new Nocycle_a_2_00020_2H1("h1_0");
        a1_main2.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.i1_0 = new Nocycle_a_2_00020_2I1("i1_0");
        a1_main2.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.i1_0.j1_0 = new Nocycle_a_2_00020_2J1("j1_0");
        a1_main2.add();
        a1_main2.b1_0.add();
        a1_main2.b1_0.c1_0.add();
        a1_main2.b1_0.c1_0.d1_0.add();
        a1_main2.b1_0.c1_0.d1_0.e1_0.add();
        a1_main2.b1_0.c1_0.d1_0.e1_0.f1_0.add();
        a1_main2.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.add();
        a1_main2.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.add();
        a1_main2.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.i1_0.add();
        a1_main2.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.i1_0.j1_0.add();

        int result2 = a1_main2.sum + a1_main2.b1_0.sum + a1_main2.b1_0.c1_0.sum + a1_main2.b1_0.c1_0.d1_0.sum + a1_main2.b1_0.c1_0.d1_0.e1_0.sum + a1_main2.b1_0.c1_0.d1_0.e1_0.f1_0.sum + a1_main2.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.sum + a1_main2.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.sum + a1_main2.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.i1_0.sum + a1_main2.b1_0.c1_0.d1_0.e1_0.f1_0.g1_0.h1_0.i1_0.j1_0.sum;

        Nocycle_a_2_00020_3A1 a1_main3 = new Nocycle_a_2_00020_3A1("a1_main3");
        Nocycle_a_2_00020_3A2 a2_main = new Nocycle_a_2_00020_3A2("a2_main");
        Nocycle_a_2_00020_3A3 a3_main = new Nocycle_a_2_00020_3A3("a3_main");
        Nocycle_a_2_00020_3A4 a4_main = new Nocycle_a_2_00020_3A4("a4_main");
        a1_main3.b1_0 = new Nocycle_a_2_00020_3B1("b1_0");
        a1_main3.d1_0 = new Nocycle_a_2_00020_3D1("d1_0");
        a1_main3.b1_0.d2_0 = new Nocycle_a_2_00020_3D2("d2_0");

        a2_main.b2_0 = new Nocycle_a_2_00020_3B2("b2_0");
        a2_main.b2_0.c1_0 = new Nocycle_a_2_00020_3C1("c1_0");
        a2_main.b2_0.d1_0 = new Nocycle_a_2_00020_3D1("d1_0");
        a2_main.b2_0.d2_0 = new Nocycle_a_2_00020_3D2("d2_0");
        a2_main.b2_0.d3_0 = new Nocycle_a_2_00020_3D3("d3_0");
        a2_main.b2_0.c1_0.d1_0 = new Nocycle_a_2_00020_3D1("d1_0");

        a3_main.b2_0 = new Nocycle_a_2_00020_3B2("b2_0");
        a3_main.b2_0.c1_0 = new Nocycle_a_2_00020_3C1("c1_0");
        a3_main.b2_0.c1_0.d1_0 = new Nocycle_a_2_00020_3D1("d1_0");
        a3_main.b2_0.d1_0 = new Nocycle_a_2_00020_3D1("d1_0");
        a3_main.b2_0.d2_0 = new Nocycle_a_2_00020_3D2("d2_0");
        a3_main.b2_0.d3_0 = new Nocycle_a_2_00020_3D3("d3_0");

        a3_main.c2_0 = new Nocycle_a_2_00020_3C2("c2_0");
        a3_main.c2_0.d2_0 = new Nocycle_a_2_00020_3D2("d2_0");
        a3_main.c2_0.d3_0 = new Nocycle_a_2_00020_3D3("d3_0");

        a4_main.b3_0 = new Nocycle_a_2_00020_3B3("b3_0");
        a4_main.b3_0.c1_0 = new Nocycle_a_2_00020_3C1("c1_0");
        a4_main.b3_0.c1_0.d1_0 = new Nocycle_a_2_00020_3D1("d1_0");
        a4_main.c2_0 = new Nocycle_a_2_00020_3C2("c2_0");
        a4_main.c2_0.d2_0 = new Nocycle_a_2_00020_3D2("d2_0");
        a4_main.c2_0.d3_0 = new Nocycle_a_2_00020_3D3("d3_0");

        a1_main3.add();
        a2_main.add();
        a3_main.add();
        a4_main.add();
        a1_main3.b1_0.add();
        a1_main3.d1_0.add();
        a1_main3.b1_0.d2_0.add();

        a2_main.b2_0.add();
        a2_main.b2_0.c1_0.add();
        a2_main.b2_0.d1_0.add();
        a2_main.b2_0.d2_0.add();
        a2_main.b2_0.d3_0.add();
        a2_main.b2_0.c1_0.d1_0.add();

        a3_main.b2_0.add();
        a3_main.b2_0.c1_0.add();
        a3_main.b2_0.c1_0.d1_0.add();
        a3_main.b2_0.d1_0.add();
        a3_main.b2_0.d2_0.add();
        a3_main.b2_0.d3_0.add();

        a3_main.c2_0.add();
        a3_main.c2_0.d2_0.add();
        a3_main.c2_0.d3_0.add();

        a4_main.b3_0.add();
        a4_main.b3_0.c1_0.add();
        a4_main.b3_0.c1_0.d1_0.add();
        a4_main.c2_0.add();
        a4_main.c2_0.d2_0.add();
        a4_main.c2_0.d3_0.add();


        int result3 = a1_main3.sum + a2_main.sum + a3_main.sum + a4_main.sum + a1_main3.b1_0.sum + a2_main.b2_0.sum + a4_main.b3_0.sum + a2_main.b2_0.c1_0.sum + a3_main.c2_0.sum + a1_main3.d1_0.sum + a3_main.c2_0.d2_0.sum + a3_main.c2_0.d3_0.sum;

        int result = result1 + result2 + result3;

        if (result == 26225)
            checkout = true;
        //System.out.println(checkout);
    }

    public boolean check() {
        return checkout;
    }

    class Nocycle_a_2_00020_A1 {
        Nocycle_a_2_00020_B1 b1_0;
        Nocycle_a_2_00020_B2 b2_0;
        Nocycle_a_2_00020_B3 b3_0;
        Nocycle_a_2_00020_B4 b4_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_A1(String strObjectName) {
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

    class Nocycle_a_2_00020_B1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_B1(String strObjectName) {
            a = 201;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_2_00020_B2 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_B2(String strObjectName) {
            a = 202;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_2_00020_B3 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_B3(String strObjectName) {
            a = 203;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B3_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_2_00020_B4 {
        Nocycle_a_2_00020_C1 c1_0;
        Nocycle_a_2_00020_C2 c2_0;
        Nocycle_a_2_00020_C3 c3_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_B4(String strObjectName) {
            c1_0 = null;
            c2_0 = null;
            c3_0 = null;
            a = 204;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B4_"+strObjectName);
        }

        void add() {
            sum = a + c1_0.a + c2_0.a + c3_0.a;
        }
    }

    class Nocycle_a_2_00020_C1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_C1(String strObjectName) {
            a = 301;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_2_00020_C2 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_C2(String strObjectName) {
            a = 302;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_2_00020_C3 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_C3(String strObjectName) {
            a = 303;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_C3_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_2_00020_2A1 {
        Nocycle_a_2_00020_2B1 b1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_2A1(String strObjectName) {
            b1_0 = null;
            a = 101;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_A1_"+strObjectName);
        }

        void add() {
            sum = a + b1_0.a;
        }
    }

    class Nocycle_a_2_00020_2B1 {
        Nocycle_a_2_00020_2C1 c1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_2B1(String strObjectName) {
            c1_0 = null;
            a = 201;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_B1_"+strObjectName);
        }

        void add() {
            sum = a + c1_0.a;
        }
    }

    class Nocycle_a_2_00020_2C1 {
        Nocycle_a_2_00020_2D1 d1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_2C1(String strObjectName) {
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

    class Nocycle_a_2_00020_2D1 {
        Nocycle_a_2_00020_2E1 e1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_2D1(String strObjectName) {
            e1_0 = null;
            a = 401;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D1_"+strObjectName);
        }

        void add() {
            sum = a + e1_0.a;
        }
    }

    class Nocycle_a_2_00020_2E1 {
        Nocycle_a_2_00020_2F1 f1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_2E1(String strObjectName) {
            f1_0 = null;
            a = 501;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_E1_"+strObjectName);
        }

        void add() {
            sum = a + f1_0.a;
        }
    }

    class Nocycle_a_2_00020_2F1 {
        Nocycle_a_2_00020_2G1 g1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_2F1(String strObjectName) {
            g1_0 = null;
            a = 601;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_F1_"+strObjectName);
        }

        void add() {
            sum = a + g1_0.a;
        }
    }

    class Nocycle_a_2_00020_2G1 {
        Nocycle_a_2_00020_2H1 h1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_2G1(String strObjectName) {
            h1_0 = null;
            a = 701;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_G1_"+strObjectName);
        }

        void add() {
            sum = a + h1_0.a;
        }
    }

    class Nocycle_a_2_00020_2H1 {
        Nocycle_a_2_00020_2I1 i1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_2H1(String strObjectName) {
            i1_0 = null;
            a = 801;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_H1_"+strObjectName);
        }

        void add() {
            sum = a + i1_0.a;
        }
    }

    class Nocycle_a_2_00020_2I1 {
        Nocycle_a_2_00020_2J1 j1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_2I1(String strObjectName) {
            j1_0 = null;
            a = 901;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_I1_"+strObjectName);
        }

        void add() {
            sum = a + j1_0.a;
        }
    }

    class Nocycle_a_2_00020_2J1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_2J1(String strObjectName) {
            a = 1001;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_J1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_2_00020_3A1 {
        Nocycle_a_2_00020_3B1 b1_0;
        Nocycle_a_2_00020_3D1 d1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_3A1(String strObjectName) {
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

    class Nocycle_a_2_00020_3A2 {
        Nocycle_a_2_00020_3B2 b2_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_3A2(String strObjectName) {
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

    class Nocycle_a_2_00020_3A3 {
        Nocycle_a_2_00020_3B2 b2_0;
        Nocycle_a_2_00020_3C2 c2_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_3A3(String strObjectName) {
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

    class Nocycle_a_2_00020_3A4 {
        Nocycle_a_2_00020_3B3 b3_0;
        Nocycle_a_2_00020_3C2 c2_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_3A4(String strObjectName) {
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

    class Nocycle_a_2_00020_3B1 {
        Nocycle_a_2_00020_3D2 d2_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_3B1(String strObjectName) {
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

    class Nocycle_a_2_00020_3B2 {
        Nocycle_a_2_00020_3C1 c1_0;
        Nocycle_a_2_00020_3D1 d1_0;
        Nocycle_a_2_00020_3D2 d2_0;
        Nocycle_a_2_00020_3D3 d3_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_3B2(String strObjectName) {
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

    class Nocycle_a_2_00020_3B3 {
        Nocycle_a_2_00020_3C1 c1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_3B3(String strObjectName) {
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

    class Nocycle_a_2_00020_3C1 {
        Nocycle_a_2_00020_3D1 d1_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_3C1(String strObjectName) {
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

    class Nocycle_a_2_00020_3C2 {
        Nocycle_a_2_00020_3D2 d2_0;
        Nocycle_a_2_00020_3D3 d3_0;
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_3C2(String strObjectName) {
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

    class Nocycle_a_2_00020_3D1 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_3D1(String strObjectName) {
            a = 401;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D1_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_2_00020_3D2 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_3D2(String strObjectName) {
            a = 402;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D2_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }

    class Nocycle_a_2_00020_3D3 {
        int a;
        int sum;
        String strObjectName;

        Nocycle_a_2_00020_3D3(String strObjectName) {
            a = 403;
            sum = 0;
            this.strObjectName = strObjectName;
//        System.out.println("RC-Testing_Construction_D3_"+strObjectName);
        }

        void add() {
            sum = a + a;
        }
    }
}

public class Nocycle_am_2_00030 {

    public static void main(String[] args) {
        rc_testcase_main_wrapper();
        Runtime.getRuntime().gc();
        rc_testcase_main_wrapper();

    }

    private static void rc_testcase_main_wrapper() {
        ThreadRc_Nocycle_am_2_00010B A1_Nocycle_am_2_00010 = new ThreadRc_Nocycle_am_2_00010B();
        ThreadRc_Nocycle_am_2_00010B A2_Nocycle_am_2_00010 = new ThreadRc_Nocycle_am_2_00010B();
        ThreadRc_Nocycle_am_2_00010B A3_Nocycle_am_2_00010 = new ThreadRc_Nocycle_am_2_00010B();
        ThreadRc_Nocycle_am_2_00010B A4_Nocycle_am_2_00010 = new ThreadRc_Nocycle_am_2_00010B();
        ThreadRc_Nocycle_am_2_00010B A5_Nocycle_am_2_00010 = new ThreadRc_Nocycle_am_2_00010B();
        ThreadRc_Nocycle_am_2_00010B A6_Nocycle_am_2_00010 = new ThreadRc_Nocycle_am_2_00010B();
        ThreadRc_Nocycle_am_2_00020B A1_Nocycle_am_2_00020 = new ThreadRc_Nocycle_am_2_00020B();
        ThreadRc_Nocycle_am_2_00020B A2_Nocycle_am_2_00020 = new ThreadRc_Nocycle_am_2_00020B();
        ThreadRc_Nocycle_am_2_00020B A3_Nocycle_am_2_00020 = new ThreadRc_Nocycle_am_2_00020B();
        ThreadRc_Nocycle_am_2_00020B A4_Nocycle_am_2_00020 = new ThreadRc_Nocycle_am_2_00020B();
        ThreadRc_Nocycle_am_2_00020B A5_Nocycle_am_2_00020 = new ThreadRc_Nocycle_am_2_00020B();
        ThreadRc_Nocycle_am_2_00020B A6_Nocycle_am_2_00020 = new ThreadRc_Nocycle_am_2_00020B();

        A1_Nocycle_am_2_00010.start();
        A2_Nocycle_am_2_00010.start();
        A3_Nocycle_am_2_00010.start();
        A4_Nocycle_am_2_00010.start();
        A5_Nocycle_am_2_00010.start();
        A6_Nocycle_am_2_00010.start();
        A1_Nocycle_am_2_00020.start();
        A2_Nocycle_am_2_00020.start();
        A3_Nocycle_am_2_00020.start();
        A4_Nocycle_am_2_00020.start();
        A5_Nocycle_am_2_00020.start();
        A6_Nocycle_am_2_00020.start();

        try {
            A1_Nocycle_am_2_00010.join();
            A2_Nocycle_am_2_00010.join();
            A3_Nocycle_am_2_00010.join();
            A4_Nocycle_am_2_00010.join();
            A5_Nocycle_am_2_00010.join();
            A6_Nocycle_am_2_00010.join();
            A1_Nocycle_am_2_00020.join();
            A2_Nocycle_am_2_00020.join();
            A3_Nocycle_am_2_00020.join();
            A4_Nocycle_am_2_00020.join();
            A5_Nocycle_am_2_00020.join();
            A6_Nocycle_am_2_00020.join();

        } catch (InterruptedException e) {
        }
        if (A1_Nocycle_am_2_00010.check() && A2_Nocycle_am_2_00010.check() && A3_Nocycle_am_2_00010.check() && A4_Nocycle_am_2_00010.check() && A5_Nocycle_am_2_00010.check() && A6_Nocycle_am_2_00010.check() && A1_Nocycle_am_2_00020.check() && A2_Nocycle_am_2_00020.check() && A3_Nocycle_am_2_00020.check() && A4_Nocycle_am_2_00020.check() && A5_Nocycle_am_2_00020.check() && A6_Nocycle_am_2_00020.check())
            System.out.println("ExpectResult");
    }
}

// EXEC:%maple  %f %build_option -o %n.so
// EXEC:%run %n.so %n %run_option | compare %f
// ASSERT: scan-full ExpectResult\nExpectResult\n